
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, 
                                                            XnUserID nId, void* pCookie);
void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, 
                                                            XnUserID nId, 
                                                            XnCalibrationStatus eStatus, 
                                                            void* pCookie);
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, 
                                                            const XnChar* strPose, XnUserID nId, 
                                                            void* pCookie);