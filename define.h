#ifndef DEFINE_H_
#define DEFINE_H_

#include <XnCppWrapper.h>

#define CHECK_RC(rc, what)                                             \
    if (rc != XN_STATUS_OK)                                            \
    {                                                                                                                                         \
        printf("%s failed: %s\n", what, xnGetStatusString(rc));        \
        return rc;                                                     \
    }

xn::Context        *g_context; 
xn::DepthGenerator *g_depthGenerator;
xn::UserGenerator  *g_userGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";
XnUserID g_trackedUser = NULL;

float g_smoothing = 0.1;

typedef enum
{
    IN_SESSION,
    NOT_IN_SESSION,
    QUICK_REFOCUS
} SessionState;

SessionState g_sessionState = NOT_IN_SESSION;

#endif