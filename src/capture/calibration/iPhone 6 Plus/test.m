KK_left = stereoParams.CameraParameters1.IntrinsicMatrix;
KK_left = KK_left';

KK_right = stereoParams.CameraParameters2.IntrinsicMatrix;
KK_right = KK_right';

R = stereoParams.RotationOfCamera2;
T = stereoParams.TranslationOfCamera2;
T = T';