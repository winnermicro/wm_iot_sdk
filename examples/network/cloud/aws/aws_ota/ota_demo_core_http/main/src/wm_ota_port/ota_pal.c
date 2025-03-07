#include <stdio.h>
#include "ota_pal_posix.h"

#include "FreeRTOS_POSIX.h"
#include "wm_ota_ops.h"

static wm_ota_ops_ctx_t ota_ops_ctx = { 0 };
static OtaImageState_t g_State      = OtaPalImageStateValid;

/* Attempt to create a new receive file for the file chunks as they come in. */
OtaPalStatus_t otaPal_CreateFileForRx(OtaFileContext_t *const pFileContext)
{
    int ret = WM_ERR_SUCCESS;
    wm_log_info("otaPal_CreateFileForRx");

    if ((NULL == pFileContext) || (NULL == pFileContext->pFilePath)) {
        wm_log_error("pFileContext NULL");
        return OTA_PAL_COMBINE_ERR(OtaPalRxFileCreateFailed, 0);
    }

    ret = wm_ota_ops_get_ota_partition(&ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wm_ota_ops_get_ota_partition error: %X", ret);
        return OTA_PAL_COMBINE_ERR(OtaPalRxFileCreateFailed, 0);
    }

    ret = wm_ota_ops_begin(&ota_ops_ctx, OTA_SIZE_UNKNOWN);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wm_ota_ops_begin error: %X", ret);
        return OTA_PAL_COMBINE_ERR(OtaPalRxFileCreateFailed, 0);
    }

    pFileContext->pFile = (otaconfigOTA_FILE_TYPE *)&ota_ops_ctx;

    wm_log_info("aws ota begin succeeded");

    return OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);
}

int16_t otaPal_WriteBlock(OtaFileContext_t *const pFileContext, uint32_t iOffset, uint8_t *const pacData, uint32_t iBlockSize)
{
    int ret = WM_ERR_SUCCESS;
    wm_log_info("otaPal_WriteBlock");

    if (pFileContext != NULL && pacData && iBlockSize) {
        ret = wm_ota_ops_write(&ota_ops_ctx, pacData, iBlockSize);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("wm_ota_ops_write error: %X", ret);
            return -1;
        }
    } else {
        wm_log_info("Invalid OTA Context");
        return -1;
    }

    return iBlockSize;
}

/* Verify the signature of the specified file. */
OtaPalStatus_t otaPal_CheckFileSignature(OtaFileContext_t *const pFileContext)
{
    OtaPalStatus_t result = OtaPalSuccess;
    // not signature.
    wm_log_info("otaPal_CheckFileSignature");

    return result;
}

/* Close the specified file. This shall authenticate the file if it is marked as secure. */
OtaPalStatus_t otaPal_CloseFile(OtaFileContext_t *const pFileContext)
{
    int ret                    = WM_ERR_SUCCESS;
    OtaPalMainStatus_t mainErr = OtaPalSuccess;

    wm_log_info("otaPal_CloseFile");

    if (pFileContext == NULL) {
        return OTA_PAL_COMBINE_ERR(OtaPalFileClose, 0);
    }

    ret = wm_ota_ops_end(&ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wm_ota_ops_end error: %X", ret);
        mainErr = OtaPalSignatureCheckFailed;
    }

    return OTA_PAL_COMBINE_ERR(mainErr, 0);
}

OtaPalStatus_t otaPal_ResetDevice(OtaFileContext_t *const pFileContext)
{
    int ret = WM_ERR_SUCCESS;
    (void)pFileContext;

    wm_log_info("otaPal_ResetDevice");

    /* Short delay for debug log output before reset. */
    vTaskDelay(1000);

    ret = wm_ota_ops_reboot();
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wm_ota_ops_reboot error: %X", ret);
        return OTA_PAL_COMBINE_ERR(OtaPalActivateFailed, 0);
    }

    return OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);
}

OtaPalStatus_t otaPal_ActivateNewImage(OtaFileContext_t *const pFileContext)
{
    (void)pFileContext;

    wm_log_info("otaPal_ActivateNewImage");

    otaPal_ResetDevice(pFileContext);
    return OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);
}
OtaPalImageState_t otaPal_GetPlatformImageState(OtaFileContext_t *const pFileContext)
{
    OtaPalImageState_t eImageState = OtaPalImageStateInvalid;

    (void)pFileContext;
    wm_log_info("otaPal_GetPlatformImageState");

    eImageState = g_State;

    return eImageState;
}
/* Abort receiving the specified OTA update by closing the file. */
OtaPalStatus_t otaPal_Abort(OtaFileContext_t *const pFileContext)
{
    int ret                = WM_ERR_SUCCESS;
    OtaPalStatus_t ota_ret = OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);

    ret = wm_ota_ops_abort(&ota_ops_ctx);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("wm_ota_ops_abort error: %X", ret);
        return OTA_PAL_COMBINE_ERR(OtaPalAbortFailed, 0);
    }

    return ota_ret;
}

OtaPalStatus_t otaPal_SetPlatformImageState(OtaFileContext_t *const pFileContext, OtaImageState_t eState)
{
    (void)pFileContext;
    g_State = eState;

    return OTA_PAL_COMBINE_ERR(OtaPalSuccess, 0);
}
