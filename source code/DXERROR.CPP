//***********************************************************************
// File Name: dxerror.cpp
//
// Purpose:  Prints message boxes based on HRESULTS passed to functions
//			 and performs any cleanup, if necessary.
//
// Last Modified:  Date 3/08/98      Author - Max Szlagor     
//
//***********************************************************************

#include <windows.h>
#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include "dxerror.h"


//***********************************************************************
// Function: DisplayDDrawError
//
// Purpose:  Displays a detailed error message given an HRESULT passed
//			 as a result of a failed Direct Draw method.
//
// Parameters: HWND Hwnd		Handle to the window
//			   HRESULT	hRet	The DirectDraw method return code
//
// Returns: void
//
// Last Modified:  Date 3/08/99      Author - Max Szlagor     
//
//***********************************************************************
void DisplayDDError(
					HWND Hwnd,
					HRESULT hRet
				    )
{
	switch(hRet)
	{
	case DDERR_ALREADYINITIALIZED:

		MessageBox(Hwnd, "The object has already been initialized.", "DDERR_ALREADYINITIALIZED", MB_OKCANCEL);

		break;

	case DDERR_BLTFASTCANTCLIP:

		MessageBox(Hwnd, "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface4::BltFast method.", "DDERR_BLTFASTCANTCLIP", MB_OKCANCEL);

		break;

	case DDERR_CANNOTATTACHSURFACE:

		MessageBox(Hwnd, "A surface cannot be attached to another requested surface.", "DDERR_CANNOTATTACHSURFACE", MB_OKCANCEL);

		break;

	case DDERR_CANNOTDETACHSURFACE:

		MessageBox(Hwnd, "A surface cannot be detached from another requested surface.", "DDERR_CANNOTDETACHSURFACE", MB_OKCANCEL);

		break;

	case DDERR_CANTCREATEDC:

		MessageBox(Hwnd, "Windows can not create any more device contexts (DCs), or a DC was requested for a palette-indexed surface when the surface had no palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC). ", "DDERR_CANTCREATEDC", MB_OKCANCEL);

		break;

	case DDERR_CANTDUPLICATE:

		MessageBox(Hwnd, "Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.", "DDERR_CANTDUPLICATE", MB_OKCANCEL);

		break;

	case DDERR_CANTLOCKSURFACE:

		MessageBox(Hwnd, "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.", "DDERR_CANTLOCKSURFACE", MB_OKCANCEL);

		break;

	case DDERR_CANTPAGELOCK:

		MessageBox(Hwnd, "An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface.", "DDERR_CANTPAGELOCK", MB_OKCANCEL);

		break;

	case DDERR_CANTPAGEUNLOCK:

		MessageBox(Hwnd, "An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface.", "DDERR_CANTPAGEUNLOCK", MB_OKCANCEL);

		break;

	case DDERR_CLIPPERISUSINGHWND:

		MessageBox(Hwnd, "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.", "DDERR_CLIPPERISUSINGHWND", MB_OKCANCEL);

		break;

	case DDERR_COLORKEYNOTSET:

		MessageBox(Hwnd, "No source color key is specified for this operation.", "DDERR_COLORKEYNOTSET", MB_OKCANCEL);

		break;

	case DDERR_CURRENTLYNOTAVAIL:

		MessageBox(Hwnd, "No support is currently available.", "DDERR_CURRENTLYNOTAVAIL", MB_OKCANCEL);

		break;

	case DDERR_DCALREADYCREATED:

		MessageBox(Hwnd, "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.", "DDERR_DCALREADYCREATED", MB_OKCANCEL);

		break;

	case DDERR_DEVICEDOESNTOWNSURFACE:

		MessageBox(Hwnd, "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.", "DDERR_DEVICEDOESNTOWNSURFACE", MB_OKCANCEL);

		break;

	case DDERR_DIRECTDRAWALREADYCREATED:
		
		MessageBox(Hwnd, "A DirectDraw object representing this driver has already been created for this process.", "DDERR_DIRECTDRAWALREADYCREATED", MB_OKCANCEL);

		break;

	case DDERR_EXCEPTION:
		
		MessageBox(Hwnd, "An exception was encountered while performing the requested operation.", "DDERR_EXCEPTION", MB_OKCANCEL);

		break;

	case DDERR_EXCLUSIVEMODEALREADYSET:
		
		MessageBox(Hwnd, "An attempt was made to set the cooperative level when it was already set to exclusive.", "DDERR_EXCLUSIVEMODEALREADYSET", MB_OKCANCEL);

		break;

/*	case DDERR_EXPIRED:
		
		MessageBox(Hwnd, "The data has expired and is therefore no longer valid.", "DDERR_EXPIRED", MB_OKCANCEL);

		break;
*/
	case DDERR_GENERIC:
		
		MessageBox(Hwnd, "There is an undefined error condition.", "DDERR_GENERIC", MB_OKCANCEL);

		break;

	case DDERR_HEIGHTALIGN:
		
		MessageBox(Hwnd, "The height of the provided rectangle is not a multiple of the required alignment.", "DDERR_HEIGHTALIGN", MB_OKCANCEL);

		break;

	case DDERR_HWNDALREADYSET:
		
		MessageBox(Hwnd, "The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.", "DDERR_HWNDALREADYSET", MB_OKCANCEL);

		break;


	case DDERR_HWNDSUBCLASSED:

		MessageBox(Hwnd, "DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed.", "DDERR_HWNDSUBCLASSED", MB_OKCANCEL);

		break;

	case DDERR_IMPLICITLYCREATED:

		MessageBox(Hwnd, "The surface cannot be restored because it is an implicitly created surface.", "DDERR_IMPLICITLYCREATED", MB_OKCANCEL);

		break;

	case DDERR_INCOMPATIBLEPRIMARY:

		MessageBox(Hwnd, "The primary surface creation request does not match with the existing primary surface.", "DDERR_INCOMPATIBLEPRIMARY", MB_OKCANCEL);

		break;

	case DDERR_INVALIDCAPS:

		MessageBox(Hwnd, "One or more of the capability bits passed to the callback function are incorrect.", "DDERR_INVALIDCAPS", MB_OKCANCEL);

		break;

	case DDERR_INVALIDCLIPLIST:

		MessageBox(Hwnd, "DirectDraw does not support the provided clip list.", "DDERR_INVALIDCLIPLIST", MB_OKCANCEL);

		break;

	case DDERR_INVALIDDIRECTDRAWGUID:

		MessageBox(Hwnd, "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.", "DDERR_INVALIDDIRECTDRAWGUID", MB_OKCANCEL);

		break;

	case DDERR_INVALIDMODE:

		MessageBox(Hwnd, "DirectDraw does not support the requested mode.", "DDERR_INVALIDMODE", MB_OKCANCEL);

		break;

	case DDERR_INVALIDOBJECT:

		MessageBox(Hwnd, "DirectDraw received a pointer that was an invalid DirectDraw object.", "DDERR_INVALIDOBJECT", MB_OKCANCEL);

		break;

	case DDERR_INVALIDPARAMS:

		MessageBox(Hwnd, "One or more of the parameters passed to the method are incorrect.", "DDERR_INVALIDPARAMS", MB_OKCANCEL);

		break;

	case DDERR_INVALIDPIXELFORMAT:

		MessageBox(Hwnd, "The pixel format was invalid as specified.", "DDERR_INVALIDPIXELFORMAT", MB_OKCANCEL);

		break;

	case DDERR_INVALIDPOSITION:

		MessageBox(Hwnd, "The position of the overlay on the destination is no longer legal.", "DDERR_INVALIDPOSITION", MB_OKCANCEL);

		break;

	case DDERR_INVALIDRECT:

		MessageBox(Hwnd, "The provided rectangle was invalid.", "DDERR_INVALIDRECT", MB_OKCANCEL);

		break;
/*
	case DDERR_INVALIDSTREAM:

		MessageBox(Hwnd, "The specified stream contains invalid data.", "DDERR_INVALIDSTREAM", MB_OKCANCEL);

		break;
*/
	case DDERR_INVALIDSURFACETYPE:

		MessageBox(Hwnd, "The requested operation could not be performed because the surface was of the wrong type.", "DDERR_INVALIDSURFACETYPE", MB_OKCANCEL);

		break;

	case DDERR_LOCKEDSURFACES:

		MessageBox(Hwnd, "One or more surfaces are locked, causing the failure of the requested operation.", "DDERR_LOCKEDSURFACES", MB_OKCANCEL);

		break;

	case DDERR_MOREDATA:

		MessageBox(Hwnd, "There is more data available than the specified buffer size can hold.", "DDERR_MOREDATA", MB_OKCANCEL);

		break;

	case DDERR_NO3D:

		MessageBox(Hwnd, "No 3-D hardware or emulation is present.", "DDERR_NO3D", MB_OKCANCEL);

		break;

	case DDERR_NOALPHAHW:

		MessageBox(Hwnd, "No alpha acceleration hardware is present or available, causing the failure of the requested operation.", "DDERR_NOALPHAHW", MB_OKCANCEL);

		break;

	case DDERR_NOBLTHW:

		MessageBox(Hwnd, "No blitter hardware is present.", "DDERR_NOBLTHW", MB_OKCANCEL);

		break;

	case DDERR_NOCLIPLIST:

		MessageBox(Hwnd, "No clip list is available.", "DDERR_NOCLIPLIST", MB_OKCANCEL);

		break;

	case DDERR_NOCLIPPERATTACHED:

		MessageBox(Hwnd, "No DirectDrawClipper object is attached to the surface object.", "DDERR_NOCLIPPERATTACHED", MB_OKCANCEL);

		break;

	case DDERR_NOCOLORCONVHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no color-conversion hardware is present or available.", "DDERR_NOCOLORCONVHW", MB_OKCANCEL);

		break;

	case DDERR_NOCOLORKEY:

		MessageBox(Hwnd, "The surface does not currently have a color key.", "DDERR_NOCOLORKEY", MB_OKCANCEL);

		break;

	case DDERR_NOCOLORKEYHW:

		MessageBox(Hwnd, "The operation cannot be carried out because there is no hardware support for the destination color key.", "DDERR_NOCOLORKEYHW", MB_OKCANCEL);

		break;

	case DDERR_NOCOOPERATIVELEVELSET:

		MessageBox(Hwnd, "A create function is called without the IDirectDraw4::SetCooperativeLevel method being called.", "DDERR_NOCOOPERATIVELEVELSET", MB_OKCANCEL);

		break;

	case DDERR_NODC:

		MessageBox(Hwnd, "No DC has ever been created for this surface.", "DDERR_NODC", MB_OKCANCEL);

		break;

	case DDERR_NODDROPSHW:

		MessageBox(Hwnd, "No DirectDraw raster operation (ROP) hardware is available.", "DDERR_NODDROPSHW", MB_OKCANCEL);

		break;

	case DDERR_NODIRECTDRAWHW:

		MessageBox(Hwnd, "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.", "DDERR_NODIRECTDRAWHW", MB_OKCANCEL);

		break;

	case DDERR_NODIRECTDRAWSUPPORT:

		MessageBox(Hwnd, "DirectDraw support is not possible with the current display driver.", "DDERR_NODIRECTDRAWSUPPORT", MB_OKCANCEL);

		break;

	case DDERR_NOEMULATION:

		MessageBox(Hwnd, "Software emulation is not available.", "DDERR_NOEMULATION", MB_OKCANCEL);

		break;

	case DDERR_NOEXCLUSIVEMODE:

		MessageBox(Hwnd, "The operation requires the application to have exclusive mode, but the application does not have exclusive mode.", "DDERR_NOEXCLUSIVEMODE", MB_OKCANCEL);

		break;


	case DDERR_NOFLIPHW:

		MessageBox(Hwnd, "Flipping visible surfaces is not supported.", "DDERR_NOFLIPHW", MB_OKCANCEL);

		break;

//	case DDERR_NOFOCUSWINDOW:

//		MessageBox(Hwnd, "An attempt was made to create or set a device window without first setting the focus window.", "DDERR_NOFOCUSWINDOW", MB_OKCANCEL);

//		break;

	case DDERR_NOGDI:

		MessageBox(Hwnd, "No GDI is present.", "DDERR_NOGDI", MB_OKCANCEL);

		break;

	case DDERR_NOHWND:

		MessageBox(Hwnd, "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.", "DDERR_NOHWND", MB_OKCANCEL);

		break;


	case DDERR_NOMIPMAPHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no mipmap capable texture mapping hardware is present or available.", "DDERR_NOMIPMAPHW", MB_OKCANCEL);

		break;


	case DDERR_NOMIRRORHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no mirroring hardware is present or available.", "DDERR_NOMIRRORHW", MB_OKCANCEL);

		break;

	case DDERR_NONONLOCALVIDMEM:

		MessageBox(Hwnd, "An attempt was made to allocate non-local video memory from a device that does not support non-local video memory.", "DDERR_NONONLOCALVIDMEM", MB_OKCANCEL);

		break;

	case DDERR_NOOPTIMIZEHW:

		MessageBox(Hwnd, "The device does not support optimized surfaces.", "DDERR_NOOPTIMIZEHW", MB_OKCANCEL);

		break;

	case DDERR_NOOVERLAYDEST:

		MessageBox(Hwnd, "The IDirectDrawSurface4::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface4::UpdateOverlay method has not been called on to establish a destination.", "DDERR_NOOVERLAYDEST", MB_OKCANCEL);

		break;

	case DDERR_NOOVERLAYHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no overlay hardware is present or available.", "DDERR_NOOVERLAYHW", MB_OKCANCEL);

		break;

	case DDERR_NOPALETTEATTACHED:

		MessageBox(Hwnd, "No palette object is attached to this surface.", "DDERR_NOPALETTEATTACHED", MB_OKCANCEL);

		break;

	case DDERR_NOPALETTEHW:

		MessageBox(Hwnd, "There is no hardware support for 16- or 256-color palettes.", "DDERR_NOPALETTEHW", MB_OKCANCEL);

		break;

	case DDERR_NORASTEROPHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no appropriate raster operation hardware is present or available.", "DDERR_NORASTEROPHW", MB_OKCANCEL);

		break;

	case DDERR_NOROTATIONHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no rotation hardware is present or available.", "DDERR_NOROTATIONHW", MB_OKCANCEL);

		break;

	case DDERR_NOSTRETCHHW:

		MessageBox(Hwnd, "The operation cannot be carried out because there is no hardware support for stretching.", "DDERR_NOSTRETCHHW", MB_OKCANCEL);

		break;

	case DDERR_NOT4BITCOLOR:

		MessageBox(Hwnd, "The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette.", "DDERR_NOT4BITCOLOR", MB_OKCANCEL);

		break;


	case DDERR_NOT4BITCOLORINDEX:

		MessageBox(Hwnd, "The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette.", "DDERR_NOT4BITCOLORINDEX", MB_OKCANCEL);

		break;

	case DDERR_NOT8BITCOLOR:

		MessageBox(Hwnd, "The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette.", "DDERR_NOT8BITCOLOR", MB_OKCANCEL);

		break;

	case DDERR_NOTAOVERLAYSURFACE:

		MessageBox(Hwnd, "An overlay component is called for a non-overlay surface.", "DDERR_NOTAOVERLAYSURFACE", MB_OKCANCEL);

		break;

	case DDERR_NOTEXTUREHW:

		MessageBox(Hwnd, "The operation cannot be carried out because no texture-mapping hardware is present or available.", "DDERR_NOTEXTUREHW", MB_OKCANCEL);

		break;

	case DDERR_NOTFLIPPABLE:

		MessageBox(Hwnd, "An attempt has been made to flip a surface that cannot be flipped.", "DDERR_NOTFLIPPABLE", MB_OKCANCEL);

		break;

	case DDERR_NOTFOUND:

		MessageBox(Hwnd, "The requested item was not found.", "DDERR_NOTFOUND", MB_OKCANCEL);

		break;

	case DDERR_NOTINITIALIZED:

		MessageBox(Hwnd, "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.", "DDERR_NOTINITIALIZED", MB_OKCANCEL);

		break;

	case DDERR_NOTLOADED:

		MessageBox(Hwnd, "The surface is an optimized surface, but it has not yet been allocated any memory.", "DDERR_NOTLOADED", MB_OKCANCEL);

		break;

	case DDERR_NOTLOCKED:

		MessageBox(Hwnd, "An attempt is made to unlock a surface that was not locked.", "DDERR_NOTLOCKED", MB_OKCANCEL);

		break;

	case DDERR_NOTPAGELOCKED:

		MessageBox(Hwnd, "An attempt is made to page unlock a surface with no outstanding page locks.", "DDERR_NOTPAGELOCKED", MB_OKCANCEL);

		break;

	case DDERR_NOTPALETTIZED:

		MessageBox(Hwnd, "The surface being used is not a palette-based surface.", "DDERR_NOTPALETTIZED", MB_OKCANCEL);

		break;

	case DDERR_NOVSYNCHW:

		MessageBox(Hwnd, "The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations.", "DDERR_NOVSYNCHW", MB_OKCANCEL);

		break;

	case DDERR_NOZBUFFERHW:

		MessageBox(Hwnd, "The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers.", "DDERR_NOZBUFFERHW", MB_OKCANCEL);

		break;

	case DDERR_NOZOVERLAYHW:

		MessageBox(Hwnd, "The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays.", "DDERR_NOZOVERLAYHW", MB_OKCANCEL);

		break;

	case DDERR_OUTOFCAPS:

		MessageBox(Hwnd, "The hardware needed for the requested operation has already been allocated.", "DDERR_OUTOFCAPS", MB_OKCANCEL);

		break;

	case DDERR_OUTOFMEMORY:

		MessageBox(Hwnd, "DirectDraw does not have enough memory to perform the operation.", "DDERR_OUTOFMEMORY", MB_OKCANCEL);

		break;

	case DDERR_OUTOFVIDEOMEMORY:

		MessageBox(Hwnd, "DirectDraw does not have enough display memory to perform the operation.", "DDERR_OUTOFVIDEOMEMORY", MB_OKCANCEL);

		break;
/*
	case DDERR_OVERLAPPINGRECTS:

		MessageBox(Hwnd, "Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other.", "DDERR_OVERLAPPINGRECTS", MB_OKCANCEL);

		break;
*/
	case DDERR_OVERLAYCANTCLIP:

		MessageBox(Hwnd, "The hardware does not support clipped overlays.", "DDERR_OVERLAYCANTCLIP", MB_OKCANCEL);

		break;

	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:

		MessageBox(Hwnd, "An attempt was made to have more than one color key active on an overlay.", "DDERR_OVERLAYCOLORKEYONLYONEACTIVE", MB_OKCANCEL);

		break;

	case DDERR_OVERLAYNOTVISIBLE:

		MessageBox(Hwnd, "The IDirectDrawSurface4::GetOverlayPosition method is called on a hidden overlay.", "DDERR_OVERLAYNOTVISIBLE", MB_OKCANCEL);

		break;

	case DDERR_PALETTEBUSY:

		MessageBox(Hwnd, "Access to this palette is refused because the palette is locked by another thread.", "DDERR_PALETTEBUSY", MB_OKCANCEL);

		break;

	case DDERR_PRIMARYSURFACEALREADYEXISTS:

		MessageBox(Hwnd, "This process has already created a primary surface.", "DDERR_PRIMARYSURFACEALREADYEXISTS", MB_OKCANCEL);

		break;

	case DDERR_REGIONTOOSMALL:

		MessageBox(Hwnd, "The region passed to the IDirectDrawClipper::GetClipList method is too small.", "DDERR_REGIONTOOSMALL", MB_OKCANCEL);

		break;

	case DDERR_SURFACEALREADYATTACHED:

		MessageBox(Hwnd, "An attempt was made to attach a surface to another surface to which it is already attached.", "DDERR_SURFACEALREADYATTACHED", MB_OKCANCEL);

		break;

	case DDERR_SURFACEALREADYDEPENDENT:

		MessageBox(Hwnd, "An attempt was made to make a surface a dependency of another surface to which it is already dependent.", "DDDERR_SURFACEALREADYDEPENDENT", MB_OKCANCEL);

		break;

	case DDERR_SURFACEBUSY:

		MessageBox(Hwnd, "Access to the surface is refused because the surface is locked by another thread.", "DDERR_SURFACEBUSY", MB_OKCANCEL);

		break;

	case DDERR_SURFACEISOBSCURED:

		MessageBox(Hwnd, "Access to the surface is refused because the surface is obscured.", "DDERR_SURFACEISOBSCURED", MB_OKCANCEL);

		break;

	case DDERR_SURFACELOST:

		MessageBox(Hwnd, "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface4::Restore method on this surface to restore the memory associated with it.", "DDERR_SURFACELOST", MB_OKCANCEL);

		break;

	case DDERR_SURFACENOTATTACHED:

		MessageBox(Hwnd, "The requested surface is not attached.", "DDERR_SURFACENOTATTACHED", MB_OKCANCEL);

		break;

	case DDERR_TOOBIGHEIGHT:

		MessageBox(Hwnd, "The height requested by DirectDraw is too large.", "DDERR_TOOBIGHEIGHT", MB_OKCANCEL);

		break;

	case DDERR_TOOBIGSIZE:

		MessageBox(Hwnd, "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes.", "DDERR_TOOBIGSIZE", MB_OKCANCEL);

		break;

	case DDERR_TOOBIGWIDTH:

		MessageBox(Hwnd, "The width requested by DirectDraw is too large.", "DDERR_TOOBIGWIDTH", MB_OKCANCEL);

		break;

	case DDERR_UNSUPPORTED:

		MessageBox(Hwnd, "The operation is not supported.", "DDERR_UNSUPPORTED", MB_OKCANCEL);

		break;

	case DDERR_UNSUPPORTEDFORMAT:

		MessageBox(Hwnd, "The pixel format requested is not supported by DirectDraw.", "DDERR_UNSUPPORTEDFORMAT", MB_OKCANCEL);

		break;

	case DDERR_UNSUPPORTEDMASK:

		MessageBox(Hwnd, "The bitmask in the pixel format requested is not supported by DirectDraw.", "DDERR_UNSUPPORTEDMASK", MB_OKCANCEL);

		break;

	case DDERR_UNSUPPORTEDMODE:

		MessageBox(Hwnd, "The display is currently in an unsupported mode.", "DDERR_UNSUPPORTEDMODE", MB_OKCANCEL);

		break;

	case DDERR_VERTICALBLANKINPROGRESS:

		MessageBox(Hwnd, "A vertical blank is in progress.", "DDERR_VERTICALBLANKINPROGRESS", MB_OKCANCEL);

		break;

	case DDERR_VIDEONOTACTIVE:

		MessageBox(Hwnd, "The video port is not active.", "DDERR_VIDEONOTACTIVE", MB_OKCANCEL);

		break;

	case DDERR_WASSTILLDRAWING:

		MessageBox(Hwnd, "The previous blit operation that is transferring information to or from this surface is incomplete.", "DDERR_WASSTILLDRAWING", MB_OKCANCEL);

		break;

	case DDERR_WRONGMODE:

		MessageBox(Hwnd, "This surface cannot be restored because it was created in a different mode.", "DDERR_WRONGMODE", MB_OKCANCEL);

		break;

	case DDERR_XALIGN:

		MessageBox(Hwnd, "The provided rectangle was not horizontally aligned on a required boundary.", "DDERR_XALIGN", MB_OKCANCEL);

		break;

	}	// end switch(hRet)

}	// end DisplayDDrawError


//***********************************************************************
// Function: DisplayDSoundError
//
// Purpose:  Displays a detailed error message given an HRESULT passed
//			 as a result of a failed DirectSound method.
//
// Parameters: HWND Hwnd		Handle to the window
//			   HRESULT	hRet	The DirectDraw method return code
//
// Returns: void
//
// Last Modified:  Date 3/08/99      Author - Max Szlagor     
//
//***********************************************************************
void DisplayDSoundError(
					   HWND Hwnd,
					   HRESULT hRet
					   )
{

	switch(hRet)
	{
	case DSERR_ALLOCATED:

		MessageBox(Hwnd, "The request failed because resources, such as a priority level, were already in use by another caller.", "DSERR_ALLOCATED", MB_OKCANCEL);

		break;

	case DSERR_ALREADYINITIALIZED:

		MessageBox(Hwnd, "The object is already initialized.", "DSERR_ALREADYINITIALIZED", MB_OKCANCEL);

		break;

	case DSERR_BADFORMAT:

		MessageBox(Hwnd, "The specified wave format is not supported.", "DSERR_BADFORMAT", MB_OKCANCEL);

		break;

	case DSERR_BUFFERLOST:

		MessageBox(Hwnd, "The buffer memory has been lost and must be restored.", "DSERR_BUFFERLOST", MB_OKCANCEL);

		break;

	case DSERR_CONTROLUNAVAIL:

		MessageBox(Hwnd, "The buffer control (volume, pan, and so on) requested by the caller is not available.", "DSERR_CONTROLUNAVAIL", MB_OKCANCEL);

		break;

	case DSERR_GENERIC:

		MessageBox(Hwnd, "An undetermined error occurred inside the DirectSound subsystem.", "DSERR_GENERIC", MB_OKCANCEL);

		break;

	case DSERR_INVALIDCALL:

		MessageBox(Hwnd, "This function is not valid for the current state of this object.", "DSERR_INVALIDCALL", MB_OKCANCEL);

		break;

	case DSERR_INVALIDPARAM:

		MessageBox(Hwnd, "An invalid parameter was passed to the returning function.", "DSERR_INVALIDPARAM", MB_OKCANCEL);

		break;

	case DSERR_NOAGGREGATION:

		MessageBox(Hwnd, "The object does not support aggregation.", "DSERR_NOAGGREGATION", MB_OKCANCEL);

		break;

	case DSERR_NODRIVER:

		MessageBox(Hwnd, "No sound driver is available for use.", "DSERR_NODRIVER", MB_OKCANCEL);

		break;

	case DSERR_NOINTERFACE:

		MessageBox(Hwnd, "The requested COM interface is not available.", "DSERR_NOINTERFACE", MB_OKCANCEL);

		break;

	case DSERR_OTHERAPPHASPRIO:

		MessageBox(Hwnd, "Another application has a higher priority level, preventing this call from succeeding.", "DSERR_OTHERAPPHASPRIO", MB_OKCANCEL);

		break;

	case DSERR_OUTOFMEMORY:

		MessageBox(Hwnd, "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request.", "DSERR_OUTOFMEMORY", MB_OKCANCEL);

		break;

	case DSERR_PRIOLEVELNEEDED:

		MessageBox(Hwnd, "The caller does not have the priority level required for the function to succeed.", "DSERR_PRIOLEVELNEEDED", MB_OKCANCEL);

		break;

	case DSERR_UNINITIALIZED:

		MessageBox(Hwnd, "The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called.", "DSERR_UNINITIALIZED", MB_OKCANCEL);

		break;

	case DSERR_UNSUPPORTED:

		MessageBox(Hwnd, "The function called is not supported at this time.", "DSERR_UNSUPPORTED", MB_OKCANCEL);

		break;
	}	// end switch(hRet)
}	/* end DisplayDSoundError */


//***********************************************************************
// Function: DisplayDInputError
//
// Purpose:  Displays a detailed error message given an HRESULT passed
//			 as a result of a failed DirectInput method.
//
// Parameters: HWND Hwnd		Handle to the window
//			   HRESULT	hRet	The DirectDraw method return code
//
// Returns: void
//
// Last Modified:  Date 8/13/99      Author - Max Szlagor     
//
//***********************************************************************
void DisplayDInputError(
					HWND Hwnd,
					HRESULT hRet
				    )
{
	switch(hRet)
	{
	case DI_BUFFEROVERFLOW:

		MessageBox(Hwnd, "The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.", "DI_BUFFEROVERFLOW", MB_OKCANCEL);

		break;

	case DI_DOWNLOADSKIPPED:

		MessageBox(Hwnd, "The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.", "DI_DOWNLOADSKIPPED", MB_OKCANCEL);

		break;
	
	case DI_EFFECTRESTARTED:

		MessageBox(Hwnd, "The effect was stopped, the parameters were updated, and the effect was restarted.", "DI_EFFECTRESTARTED", MB_OKCANCEL);

		break;

/*	case DI_NOEFFECT:

		MessageBox(Hwnd, "The operation had no effect. This value is equal to the S_FALSE standard COM return value.", "DI_NOEFFECT", MB_OKCANCEL);

		break; */

/*	case DI_NOTATTACHED:

		MessageBox(Hwnd, "The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value.", "DI_NOTATTACHED", MB_OKCANCEL);

		break; */

	case DI_POLLEDDEVICE:

		MessageBox(Hwnd, "The device is a polled device. As a result, device buffering will not collect any data and event notifications will not be signaled until the IDirectInputDevice2::Poll method is called.", "DI_POLLEDDEVICE", MB_OKCANCEL);

		break;

/*	case DI_PROPNOEFFECT:

		MessageBox(Hwnd, "The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value.", "DI_PROPNOEFFECT", MB_OKCANCEL);

		break; */

	case DI_TRUNCATED:

		MessageBox(Hwnd, "The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.", "DI_TRUNCATED", MB_OKCANCEL);

		break;

	case DI_TRUNCATEDANDRESTARTED:

		MessageBox(Hwnd, "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.", "DI_TRUNCATEDANDRESTARTED", MB_OKCANCEL);

		break;

	case DIERR_ACQUIRED:

		MessageBox(Hwnd, "The operation cannot be performed while the device is acquired.", "DIERR_ACQUIRED", MB_OKCANCEL);

		break;

	case DIERR_ALREADYINITIALIZED:

		MessageBox(Hwnd, "This object is already initialized.", "DIERR_ALREADYINITIALIZED", MB_OKCANCEL);

		break;

	case DIERR_BADDRIVERVER:

		MessageBox(Hwnd, "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.", "DIERR_BADDRIVERVER", MB_OKCANCEL);

		break;

	case DIERR_BETADIRECTINPUTVERSION:

		MessageBox(Hwnd, "The application was written for an unsupported prerelease version of DirectInput.", "DIERR_BETADIRECTINPUTVERSION", MB_OKCANCEL);

		break;

	case DIERR_DEVICEFULL:

		MessageBox(Hwnd, "The device is full.", "DIERR_DEVICEFULL", MB_OKCANCEL);

		break;

	case DIERR_DEVICENOTREG:

		MessageBox(Hwnd, "The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.", "DIERR_DEVICENOTREG", MB_OKCANCEL);

		break;

	case DIERR_EFFECTPLAYING:

		MessageBox(Hwnd, "The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.", "DIERR_EFFECTPLAYING", MB_OKCANCEL);

		break;

	case DIERR_HASEFFECTS:

		MessageBox(Hwnd, "The device cannot be reinitialized because there are still effects attached to it.", "DIERR_HASEFFECTS", MB_OKCANCEL);

		break;

	case DIERR_GENERIC:

		MessageBox(Hwnd, "An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.", "DIERR_GENERIC", MB_OKCANCEL);

		break;

	case DIERR_HANDLEEXISTS:

		MessageBox(Hwnd, "The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.", "DIERR_HANDLEEXISTS", MB_OKCANCEL);

		break;

	case DIERR_INCOMPLETEEFFECT:

		MessageBox(Hwnd, "The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.", "DIERR_INCOMPLETEEFFECT", MB_OKCANCEL);

		break;

	case DIERR_INPUTLOST:

		MessageBox(Hwnd, "Access to the input device has been lost. It must be reacquired.", "DIERR_INPUTLOST", MB_OKCANCEL);

		break;

	case DIERR_INVALIDPARAM:

		MessageBox(Hwnd, "An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.", "DIERR_INVALIDPARAM", MB_OKCANCEL);

		break;

	case DIERR_MOREDATA:

		MessageBox(Hwnd, "Not all the requested information fitted into the buffer.", "DIERR_MOREDATA", MB_OKCANCEL);

		break;

	case DIERR_NOAGGREGATION:

		MessageBox(Hwnd, "This object does not support aggregation.", "DIERR_NOAGGREGATION", MB_OKCANCEL);

		break;

	case DIERR_NOINTERFACE:

		MessageBox(Hwnd, "The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value.", "DIERR_NOINTERFACE", MB_OKCANCEL);

		break;

	case DIERR_NOTACQUIRED:

		MessageBox(Hwnd, "The operation cannot be performed unless the device is acquired.", "DIERR_NOTACQUIRED", MB_OKCANCEL);

		break;

	case DIERR_NOTBUFFERED:

		MessageBox(Hwnd, "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.", "DIERR_NOTBUFFERED", MB_OKCANCEL);

		break;

	case DIERR_NOTDOWNLOADED:

		MessageBox(Hwnd, "The effect is not downloaded.", "DIERR_NOTDOWNLOADED", MB_OKCANCEL);

		break;

	case DIERR_NOTEXCLUSIVEACQUIRED:

		MessageBox(Hwnd, "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.", "DIERR_NOTEXCLUSIVEACQUIRED", MB_OKCANCEL);

		break;

	case DIERR_NOTFOUND:

		MessageBox(Hwnd, "The requested object does not exist.", "DIERR_NOTFOUND", MB_OKCANCEL);

		break;

	case DIERR_NOTINITIALIZED:

		MessageBox(Hwnd, "This object has not been initialized.", "DIERR_NOTINITIALIZED", MB_OKCANCEL);

		break;

/*	case DIERR_OBJECTNOTFOUND:

		MessageBox(Hwnd, "The requested object does not exist.", "DIERR_OBJECTNOTFOUND", MB_OKCANCEL);

		break; */

	case DIERR_OLDDIRECTINPUTVERSION:

		MessageBox(Hwnd, "The application requires a newer version of DirectInput.", "DIERR_OLDDIRECTINPUTVERSION", MB_OKCANCEL);

		break;

/*	case DIERR_OTHERAPPHASPRIO:

		MessageBox(Hwnd, "Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background.", "DIERR_OTHERAPPHASPRIO", MB_OKCANCEL);

		break; */

	case DIERR_OUTOFMEMORY:

		MessageBox(Hwnd, "The DirectInput subsystem couldn't allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.", "DIERR_OUTOFMEMORY", MB_OKCANCEL);

		break;

/*	case DIERR_READONLY:

		MessageBox(Hwnd, "The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value.", "DIERR_READONLY", MB_OKCANCEL);

		break; */

/*	case DIERR_REPORTFULL:

		MessageBox(Hwnd, "More information was requested to be sent than can be sent to the device.", "DIERR_REPORTFULL", MB_OKCANCEL);

		break; */

/*	case DIERR_UNPLUGGED:

		MessageBox(Hwnd, "The operation could not be completed because the device is not plugged in.", "DIERR_UNPLUGGED", MB_OKCANCEL);

		break; */

	case DIERR_UNSUPPORTED:

		MessageBox(Hwnd, "The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.", "DIERR_UNSUPPORTED", MB_OKCANCEL);

		break;

	case E_PENDING:

		MessageBox(Hwnd, "Data is not yet available.", "E_PENDING", MB_OKCANCEL);

		break;
	}

}


void DisplayDPlayError(
					HWND Hwnd,
					HRESULT hRet
				    )
{
	switch(hRet)
	{

	case CLASS_E_NOAGGREGATION:

		MessageBox(Hwnd, "A non-NULL value was passed for the pUnkOuter parameter in DirectPlayCreate, DirectPlayLobbyCreate, or IDirectPlayLobby3::Connect.", "CLASS_E_NOAGGREGATION", MB_OKCANCEL);

		break;

	case DPERR_ACCESSDENIED:

		MessageBox(Hwnd, "The session is full, or an incorrect password was supplied.", "DPERR_ACCESSDENIED", MB_OKCANCEL);

		break;

	case DPERR_ACTIVEPLAYERS :

		MessageBox(Hwnd, "The requested operation cannot be performed because there are existing active players.", "DPERR_ACTIVEPLAYERS", MB_OKCANCEL);

		break;

	case DPERR_ALREADYINITIALIZED:

		MessageBox(Hwnd, "This object is already initialized.", "DPERR_ALREADYINITIALIZED", MB_OKCANCEL);

		break;

	case DPERR_APPNOTSTARTED:

		MessageBox(Hwnd, "The application has not been started yet.", "DPERR_APPNOTSTARTED", MB_OKCANCEL);

		break;

	case DPERR_AUTHENTICATIONFAILED:

		MessageBox(Hwnd, "The password or credentials supplied could not be authenticated.", "DPERR_AUTHENTICATIONFAILED", MB_OKCANCEL);

		break;

	case DPERR_BUFFERTOOLARGE:

		MessageBox(Hwnd, "The data buffer is too large to store.", "DPERR_BUFFERTOOLARGE", MB_OKCANCEL);

		break;

	case DPERR_BUFFERTOOSMALL:

		MessageBox(Hwnd, "The supplied buffer is not large enough to contain the requested data.", "DPERR_BUFFERTOOSMALL", MB_OKCANCEL);

		break;

	case DPERR_BUSY:

		MessageBox(Hwnd, "A message cannot be sent because the transmission medium is busy.", "DPERR_BUSY", MB_OKCANCEL);

		break;

/*	case DPERR_CANCELFAILED:

		MessageBox(Hwnd, "The message could not be canceled, possibly because it is a group message that has already been to sent to one or more members of the group.", "DPERR_CANCELFAILED", MB_OKCANCEL);

		break; */

	case DPERR_CANNOTCREATESERVER:

		MessageBox(Hwnd, "The server cannot be created for the new session.", "DPERR_CANNOTCREATESERVER", MB_OKCANCEL);

		break;

	case DPERR_CANTADDPLAYER:

		MessageBox(Hwnd, "The player cannot be added to the session.", "DPERR_CANTADDPLAYER", MB_OKCANCEL);

		break;


	case DPERR_CANTCREATEGROUP:

		MessageBox(Hwnd, "A new group cannot be created.", "DPERR_CANTCREATEGROUP", MB_OKCANCEL);

		break;

	case DPERR_CANTCREATEPLAYER:

		MessageBox(Hwnd, "A new player cannot be created.", "DPERR_CANTCREATEPLAYER", MB_OKCANCEL);

		break;

	case DPERR_CANTCREATEPROCESS:

		MessageBox(Hwnd, "Cannot start the application.", "DPERR_CANTCREATEPROCESS", MB_OKCANCEL);

		break;

	case DPERR_CANTCREATESESSION:

		MessageBox(Hwnd, "A new session cannot be created.", "DPERR_CANTCREATESESSION", MB_OKCANCEL);

		break;

	case DPERR_CANTLOADCAPI:

		MessageBox(Hwnd, "No credentials were supplied and the CryptoAPI package (CAPI) to use for cryptography services cannot be loaded.", "DPERR_CANTLOADCAPI", MB_OKCANCEL);

		break;

	case DPERR_CANTLOADSECURITYPACKAGE:

		MessageBox(Hwnd, "The software security package cannot be loaded.", "DPERR_CANTLOADSECURITYPACKAGE", MB_OKCANCEL);

		break;

	case DPERR_CANTLOADSSPI:

		MessageBox(Hwnd, "No credentials were supplied, and the Security Support Provider Interface (SSPI) that will prompt for credentials cannot be loaded.", "DPERR_CANTLOADSSPI", MB_OKCANCEL);

		break;

	case DPERR_CAPSNOTAVAILABLEYET:

		MessageBox(Hwnd, "The capabilities of the DirectPlay object have not been determined yet. This error will occur if the DirectPlay object is implemented on a connectivity solution that requires polling to determine available bandwidth and latency.", "DPERR_CAPSNOTAVAILABLEYET", MB_OKCANCEL);

		break;

/*	case DDPERR_CONNECTING:

		MessageBox(Hwnd, "No credentials were supplied, and the Security Support Provider Interface (SSPI) that will prompt for credentials cannot be loaded.", "DPERR_CONNECTING", MB_OKCANCEL);

		break; */

/*	case DPERR_CONNECTIONLOST:

		MessageBox(Hwnd, "The service provider connection was reset while data was being sent.", "DPERR_CONNECTIONLOST", MB_OKCANCEL);

		break; */

	case DPERR_ENCRYPTIONFAILED:

		MessageBox(Hwnd, "The requested information could not be digitally encrypted. Encryption is used for message privacy. This error is only relevant in a secure session.", "DPERR_ENCRYPTIONFAILED", MB_OKCANCEL);

		break;

	case DPERR_ENCRYPTIONNOTSUPPORTED:

		MessageBox(Hwnd, "The type of encryption requested in the DPSECURITYDESC structure is not available. This error is only relevant in a secure session.", "DPERR_ENCRYPTIONNOTSUPPORTED", MB_OKCANCEL);

		break;

	case DPERR_EXCEPTION:

		MessageBox(Hwnd, "An exception occurred when processing the request.", "DPERR_EXCEPTION", MB_OKCANCEL);

		break;

	case DPERR_GENERIC:

		MessageBox(Hwnd, "An undefined error condition occurred.", "DPERR_GENERIC", MB_OKCANCEL);

		break;

	case DPERR_INVALIDFLAGS:

		MessageBox(Hwnd, "The flags passed to this method are invalid.", "DPERR_INVALIDFLAGS", MB_OKCANCEL);

		break;

	case DPERR_INVALIDGROUP:

		MessageBox(Hwnd, "The group ID is not recognized as a valid group ID for this game session.", "DPERR_INVALIDGROUP", MB_OKCANCEL);

		break;

	case DPERR_INVALIDINTERFACE:

		MessageBox(Hwnd, "The interface parameter is invalid.", "DPERR_INVALIDINTERFACE", MB_OKCANCEL);

		break;

	case DPERR_INVALIDOBJECT:

		MessageBox(Hwnd, "The DirectPlay object pointer is invalid.", "DPERR_INVALIDOBJECT", MB_OKCANCEL);

		break;

	case DPERR_INVALIDPARAMS:

		MessageBox(Hwnd, "One or more of the parameters passed to the method are invalid.", "DPERR_INVALIDPARAMS", MB_OKCANCEL);

		break;

	case DPERR_INVALIDPASSWORD:

		MessageBox(Hwnd, "An invalid password was supplied when attempting to join a session that requires a password.", "DPERR_INVALIDPASSWORD", MB_OKCANCEL);

		break;

	case DPERR_INVALIDPLAYER:

		MessageBox(Hwnd, "The player ID is not recognized as a valid player ID for this game session.", "DPERR_INVALIDPLAYER", MB_OKCANCEL);

		break;

/*	case DPERR_INVALIDPRIORITY:

		MessageBox(Hwnd, "The specified priority is not within the range of allowed priorities, which is inclusively 0-65535.", "DPERR_INVALIDPRIORITY", MB_OKCANCEL);

		break; */

	case DPERR_LOGONDENIED:

		MessageBox(Hwnd, "The session could not be opened because credentials are required, and either no credentials were supplied, or the credentials were invalid.", "DPERR_LOGONDENIED", MB_OKCANCEL);

		break;

	case DPERR_NOCAPS:

		MessageBox(Hwnd, "The communication link that DirectPlay is attempting to use is not capable of this function.", "DPERR_NOCAPS", MB_OKCANCEL);

		break;

	case DPERR_NOCONNECTION:

		MessageBox(Hwnd, "No communication link was established.", "DPERR_NOCONNECTION", MB_OKCANCEL);

		break;

	case DPERR_NOINTERFACE:

		MessageBox(Hwnd, "The interface is not supported.", "DPERR_NOINTERFACE", MB_OKCANCEL);

		break;

	case DPERR_NOMESSAGES:

		MessageBox(Hwnd, "There are no messages in the receive queue.", "DPERR_NOMESSAGES", MB_OKCANCEL);

		break;

	case DPERR_NONAMESERVERFOUND:

		MessageBox(Hwnd, "No name server (host) could be found or created. A host must exist to create a player.", "DPERR_NONAMESERVERFOUND", MB_OKCANCEL);

		break;

	case DPERR_NONEWPLAYERS:

		MessageBox(Hwnd, "The session is not accepting any new players.", "DPERR_NONEWPLAYERS", MB_OKCANCEL);

		break;

	case DPERR_NOPLAYERS:

		MessageBox(Hwnd, "There are no active players in the session.", "DPERR_NOPLAYERS", MB_OKCANCEL);

		break;

	case DPERR_NOSESSIONS:

		MessageBox(Hwnd, "There are no existing sessions for this game.", "DPERR_NOSESSIONS", MB_OKCANCEL);

		break;

	case DPERR_NOTLOBBIED:

		MessageBox(Hwnd, "Returned by the IDirectPlayLobby3::Connect method if the application was not started by using the IDirectPlayLobby3::RunApplication method, or if there is no DPLCONNECTION structure currently initialized for this DirectPlayLobby object.", "DPERR_NOTLOBBIED", MB_OKCANCEL);

		break;

	case DPERR_NOTLOGGEDIN:

		MessageBox(Hwnd, "An action cannot be performed because a player or client application is not logged in. Returned by the IDirectPlay4::Send method when the client application tries to send a secure message without being logged in.", "DPERR_NOTLOGGEDIN", MB_OKCANCEL);

		break;

	case DPERR_OUTOFMEMORY:

		MessageBox(Hwnd, "There is insufficient memory to perform the requested operation.", "DPERR_OUTOFMEMORY", MB_OKCANCEL);

		break;

	case DPERR_PENDING:

		MessageBox(Hwnd, "Not an error, this return indicates that an asynchronous send has reached the point where it is successfully queued. See SendEx for more information.", "DPERR_PENDING ", MB_OKCANCEL);

		break;

	case DPERR_PLAYERLOST:

		MessageBox(Hwnd, "A player has lost the connection to the session.", "DPERR_PLAYERLOST ", MB_OKCANCEL);

		break;

	case DPERR_SENDTOOBIG:

		MessageBox(Hwnd, "The message being sent by the IDirectPlay4::Send method is too large.", "DPERR_PLAYERLOST ", MB_OKCANCEL);

		break;

	case DPERR_SESSIONLOST:

		MessageBox(Hwnd, "The connection to the session has been lost.", "DPERR_SESSIONLOST", MB_OKCANCEL);

		break;

	case DPERR_SIGNFAILED:

		MessageBox(Hwnd, "The requested information could not be digitally signed. Digital signatures are used to establish the authenticity of messages.", "DPERR_SIGNFAILED", MB_OKCANCEL);

		break;

/*	case DPERR_SIGNFAILED:

		MessageBox(Hwnd, "The requested information could not be digitally signed. Digital signatures are used to establish the authenticity of messages.", "DPERR_SIGNFAILED", MB_OKCANCEL);

		break; */

	case DPERR_TIMEOUT:

		MessageBox(Hwnd, "The operation could not be completed in the specified time.", "DPERR_TIMEOUT", MB_OKCANCEL);

		break;

	case DPERR_UNAVAILABLE:

		MessageBox(Hwnd, "The requested function is not available at this time.", "DPERR_UNAVAILABLE", MB_OKCANCEL);

		break;

	case DPERR_UNINITIALIZED:

		MessageBox(Hwnd, "The requested object has not been initialized.", "DPERR_UNINITIALIZED", MB_OKCANCEL);

		break;

	case DPERR_UNKNOWNAPPLICATION:

		MessageBox(Hwnd, "An unknown application was specified.", "DPERR_UNKNOWNAPPLICATION", MB_OKCANCEL);

		break;

/*	case DPERR_UNKNOWNMESSAGE:

		MessageBox(Hwnd, "The message ID isn't valid. Returned from IDirectPlay4::CancelMessage if the ID of the message to be cancelled is invalid.", "DPERR_UNKNOWNMESSAGE", MB_OKCANCEL);

		break; */

	case DPERR_UNSUPPORTED:

		MessageBox(Hwnd, "The function or feature is not available in this implementation or on this service provider. Returned from IDirectPlay4::GetGroupConnectionSettings and IDirectPlay4::SetGroupConnectionSettings if they are called from a session that is not a lobby session. Returned from IDirectPlay4::SendEx if the priority or timeout is set, and these are not supported by the service provider and DirectPlay protocol is not on. Returned from IDirectPlay4::GetMessageQueue if you check the send queue and this is not supported by the service provider and DirectPlay protocol is not on.", "DPERR_UNSUPPORTED", MB_OKCANCEL);

		break;

	case DPERR_USERCANCEL:

		MessageBox(Hwnd, "Can be returned in two ways. 1) The user canceled the connection process during a call to the IDirectPlay4::Open method. 2) The user clicked Cancel in one of the DirectPlay service provider dialog boxes during a call to IDirectPlay4::EnumSessions.", "DPERR_USERCANCEL", MB_OKCANCEL);

		break;

/*	case E_UNKNOWN:

		MessageBox(Hwnd, "Usually indicates a catastrophic failure of some sort; for example, the service provider cannot initialize correctly or a protocol such as IPX is not installed correctly on the machine.", "E_UNKNOWN", MB_OKCANCEL);

		break; */

		}	// end switch(hRet)
}




