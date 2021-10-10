#include <stdio.h>
#include <stdbool.h>

#include <libmng.h>

#include "mngfile.h"
#include "types.h"
#include "err.h"
#include "verbose.h"

// makes it easier to set the billions of callbacks.
// this requires that each of the callbacks follow the naming scheme of the
// callback type, replacing "mng_" with "cb_".
// e.g. "cb_memalloc" is the callback of "mng_memalloc" type.
#define SETCB(mng_handle, cb_name) \
	mng_setcb_ ## cb_name (mng_handle, cb_ ## cb_name)

// libmng requires a stupid amount of callbacks.
// here is all of them.
static mng_ptr
cb_memalloc(
	mng_size_t sz)
{
	return calloc(1, sz);
}
static void
cb_memfree(
	mng_ptr ptr,
	mng_size_t sz)
{
	free(ptr);
}
// file stream is handled outside of callbacks; do nothing.
static mng_bool
cb_openstream(
	mng_handle mh)
{
	return MNG_TRUE;
}
static mng_bool
(*cb_closestream)(
	mng_handle mh) = cb_openstream;
static mng_bool
cb_readdata(
	mng_handle mh,
	mng_ptr buf,
	mng_uint32 buf_len,
	mng_uint32p nread_ptr)
{
	struct mngfile *mfp = mng_get_userdata(mh);
	*nread_ptr = fread(buf, 1, buf_len, mfp->fp);
	return MNG_TRUE;
}
// called to get the height and width of all frames, i think?
static mng_bool
cb_processheader(
	mng_handle mh,
	mng_uint32 width,
	mng_uint32 height)
{
	struct mngfile *mfp = mng_get_userdata(mh);
	// metadata is simply cached in userdata; other functions actually
	// assign the cached data.
	mfp->cached_md.height = height;
	mfp->cached_md.width = width;
	return MNG_TRUE;
}
// this needs access to image data prior to being called.
static mng_ptr
cb_getcanvasline(
	mng_handle mh,
	mng_uint32 lineno)
{
	struct mngfile *mfp = mng_get_userdata(mh);

	/*verbf("%zu/%zu rows: %.2lf%%.\n",
		lineno, mfp->imgp->md.height,
		(double)lineno / mfp->imgp->md.height * 100);*/

	// image pointer should be populated by now.
	// the canvas style should be set to RGBA8, so we can pass the pixel
	// rows directly.
	return mfp->imgp->pixel_rows[lineno];
}
// this is called when the frame is ready to be displayed; i don't think it's
// needed because the library returns control to me when it's waiting for the
// next frame and i will be freezing the tickcount to force this.
static mng_bool
cb_refresh(
	mng_handle mh,
	mng_uint32 x, // ???
	mng_uint32 y, // ???
	mng_uint32 w,
	mng_uint32 h)
{
	return MNG_TRUE;
}
// don't return actual ticks; freeze them to be incremented manually to force
// frames when i want them.
static mng_uint32
cb_gettickcount(
	mng_handle mh)
{
	struct mngfile *mfp = mng_get_userdata(mh);
	return mfp->ticks;
}
static mng_bool
cb_settimer(
	mng_handle mh,
	mng_uint32 msecs)
{
	// this will tell libmng that the alotted time has "passed" when i ask
	// for the next frame.
	struct mngfile *mfp = mng_get_userdata(mh);
	mfp->ticks += msecs;
	return MNG_TRUE;
}

// initializes the libmng handle with all callbacks.
// if false, handle was unable to be obtained; no freeing is necessary.
static bool
init_libmng(
	struct mngfile *mfp)
{
	// pointer to the mngfile is the userdata, but it must be set before
	// every operation in these functions since it can be changed.
	mfp->mh = mng_initialize(mfp, cb_memalloc, cb_memfree, MNG_NULL);
	if (mfp->mh == MNG_NULL) {
		ERR("mng_initialize() failed.\n");
		return false;
	}
	// set all the callbacks.
	// i don't see how these could possibly fail, so i'm not going to bother
	// checking the return code.
	SETCB(mfp->mh, openstream);
	SETCB(mfp->mh, closestream);
	SETCB(mfp->mh, readdata);
	SETCB(mfp->mh, processheader);
	SETCB(mfp->mh, getcanvasline);
	SETCB(mfp->mh, refresh);
	SETCB(mfp->mh, gettickcount);
	SETCB(mfp->mh, settimer);
	// canvas style MUST be equivalent to the order of the pixel components
	// in the predefined pixel type.
	mng_set_canvasstyle(mfp->mh, MNG_CANVAS_RGBA8);

	return true;
}

enum mngf_retcode
mngf_init(
	struct mngfile *mfp,
	FILE *fp)
{
	// set file.
	mfp->fp = fp;

	// get libmng handle.
	if (!init_libmng(mfp)) {
		goto error_libmng;
	}

	// initialize data.
	mfp->ticks = 0;
	mfp->has_started_displaying = false;
	mfp->is_eof = false;
	mfp->imgp = NULL; // not needed but might help debugging.
	// in order to obtain metadata, the file must be read.
	// instead of using a flag to see if the metadata has been read yet, the
	// whole file is simply read in initialization to guarantee the cached
	// metadata is available.
	mng_retcode rc = mng_read(mfp->mh);
	if (rc != MNG_NOERROR) {
		ERR("mng_read() failed.\n");
		goto error_mngread;
	}

	return MNGF_OK;

error_mngread:
	mng_cleanup(&mfp->mh);
error_libmng:
	ERR("failed to initialize.\n");
	return MNGF_ERR;
}

void
mngf_clear(
	struct mngfile *mfp)
{
	mng_cleanup(&mfp->mh);
}

enum mngf_retcode
mngf_read_md(
	struct mngfile *mfp,
	struct metadata *mdp)
{
	*mdp = mfp->cached_md;
	return MNGF_OK;
}

enum mngf_retcode
mngf_read_img(
	struct mngfile *mfp,
	struct image *imgp)
{
	// check that eof hasn't already been reached.
	if (mfp->is_eof) {
		return MNGF_EOF;
	}

	// prepare userdata.
	mfp->imgp = imgp;
	mng_set_userdata(mfp->mh, mfp);
	verbf("new MNG frame @ %zu ticks.\n", mfp->ticks);
	// "display" frame into the image struct.
	//
	// TODO: a static array of function pointers to both display functions
	// could be used with the flag being an index to the proper function, as
	// they have identical prototypes.
	// however, the flag still needs to be set somehow, which would be
	// possible by making a wrapper for mng_display() which would set the
	// flag by accessing the userdata like a callback.
	// this seems overly convoluted just to remove a conditional branch,
	// tho lol.
	mng_retcode rc;
	if (mfp->has_started_displaying) {
		rc = mng_display_resume(mfp->mh);
	} else {
		rc = mng_display(mfp->mh);
		mfp->has_started_displaying = true;
	}
	// MNG_NOERROR is returned after the final frame has been read.
	// this function needs to return EOF when one tries to read the next
	// frame, so this has to set a flag.
	// MNG_NEEDTIMERWAIT is the "non-error" return in this case.
	if (rc == MNG_NOERROR) {
		mfp->is_eof = true;
	} else if (rc != MNG_NEEDTIMERWAIT) {
		ERR("failed to get next frame.\n");
		mfp->imgp = NULL;
		return MNGF_ERR;
	}
	mfp->imgp = NULL;
	return MNGF_OK;
}
