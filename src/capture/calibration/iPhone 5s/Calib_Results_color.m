% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 551.628479834655536 ; 552.035894082942150 ];

%-- Principal point:
cc = [ 315.288799776939982 ; 240.433097416418860 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.048284843435510 ; -0.010521464279892 ; 0.003185512020896 ; -0.001190898931098 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 2.355029183814030 ; 2.185639033266757 ];

%-- Principal point uncertainty:
cc_error = [ 1.848756202293502 ; 2.386708704235004 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.010504807864458 ; 0.042922215676587 ; 0.001229041237279 ; 0.001429002592975 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 480;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 20;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 8.573632e-01 ; 2.422798e+00 ; -5.098251e-01 ];
Tc_1  = [ 7.154351e+01 ; -1.583113e+02 ; 5.395499e+02 ];
omc_error_1 = [ 2.309680e-03 ; 4.064165e-03 ; 5.193331e-03 ];
Tc_error_1  = [ 1.828081e+00 ; 2.310305e+00 ; 2.267405e+00 ];

%-- Image #2:
omc_2 = [ 7.740619e-01 ; 2.785145e+00 ; -5.178022e-01 ];
Tc_2  = [ 9.949556e+01 ; -1.533525e+02 ; 5.213258e+02 ];
omc_error_2 = [ 1.577130e-03 ; 4.348339e-03 ; 6.309923e-03 ];
Tc_error_2  = [ 1.762320e+00 ; 2.216045e+00 ; 2.348364e+00 ];

%-- Image #3:
omc_3 = [ -6.739413e-01 ; -2.918973e+00 ; 5.723092e-01 ];
Tc_3  = [ 1.254180e+02 ; -1.481890e+02 ; 5.132182e+02 ];
omc_error_3 = [ 2.095367e-03 ; 4.419168e-03 ; 7.040753e-03 ];
Tc_error_3  = [ 1.742030e+00 ; 2.186862e+00 ; 2.373092e+00 ];

%-- Image #4:
omc_4 = [ -6.464562e-01 ; -2.816589e+00 ; 5.742731e-01 ];
Tc_4  = [ 6.799393e+01 ; -1.395339e+02 ; 5.137005e+02 ];
omc_error_4 = [ 2.122870e-03 ; 4.355803e-03 ; 6.570113e-03 ];
Tc_error_4  = [ 1.735550e+00 ; 2.184324e+00 ; 2.383854e+00 ];

%-- Image #5:
omc_5 = [ -5.972952e-01 ; -2.687671e+00 ; 4.798379e-01 ];
Tc_5  = [ 4.514725e+01 ; -1.572507e+02 ; 4.499619e+02 ];
omc_error_5 = [ 2.147429e-03 ; 4.134307e-03 ; 5.798062e-03 ];
Tc_error_5  = [ 1.533749e+00 ; 1.913751e+00 ; 2.142954e+00 ];

%-- Image #6:
omc_6 = [ -6.121951e-01 ; -2.687028e+00 ; 4.717146e-01 ];
Tc_6  = [ 2.663748e+01 ; -1.398449e+02 ; 4.228942e+02 ];
omc_error_6 = [ 2.075146e-03 ; 4.067135e-03 ; 5.638706e-03 ];
Tc_error_6  = [ 1.430870e+00 ; 1.798117e+00 ; 2.014216e+00 ];

%-- Image #7:
omc_7 = [ -5.908763e-01 ; -2.837370e+00 ; 5.674575e-01 ];
Tc_7  = [ 5.692090e+01 ; -1.134056e+02 ; 4.860769e+02 ];
omc_error_7 = [ 1.908198e-03 ; 4.299865e-03 ; 6.463492e-03 ];
Tc_error_7  = [ 1.631428e+00 ; 2.070193e+00 ; 2.250201e+00 ];

%-- Image #8:
omc_8 = [ -5.723543e-01 ; -3.009737e+00 ; 6.626221e-01 ];
Tc_8  = [ 7.400731e+01 ; -1.126524e+02 ; 5.109500e+02 ];
omc_error_8 = [ 1.925169e-03 ; 4.180727e-03 ; 7.038677e-03 ];
Tc_error_8  = [ 1.714742e+00 ; 2.171802e+00 ; 2.303081e+00 ];

%-- Image #9:
omc_9 = [ 5.063078e-01 ; 3.084848e+00 ; 1.084122e-01 ];
Tc_9  = [ 1.009489e+02 ; -1.460284e+02 ; 3.935220e+02 ];
omc_error_9 = [ 1.238599e-03 ; 4.269984e-03 ; 6.744456e-03 ];
Tc_error_9  = [ 1.352534e+00 ; 1.737084e+00 ; 1.845461e+00 ];

%-- Image #10:
omc_10 = [ 3.990497e-01 ; 3.054631e+00 ; -2.977001e-01 ];
Tc_10  = [ 1.225492e+02 ; -1.371173e+02 ; 4.368859e+02 ];
omc_error_10 = [ 1.039290e-03 ; 4.346819e-03 ; 6.606772e-03 ];
Tc_error_10  = [ 1.482315e+00 ; 1.859832e+00 ; 2.039333e+00 ];

%-- Image #11:
omc_11 = [ 2.666754e-01 ; 2.870723e+00 ; -9.765099e-01 ];
Tc_11  = [ 1.317984e+02 ; -1.226848e+02 ; 4.837643e+02 ];
omc_error_11 = [ 1.730616e-03 ; 4.311273e-03 ; 6.024568e-03 ];
Tc_error_11  = [ 1.641203e+00 ; 2.080392e+00 ; 2.052367e+00 ];

%-- Image #12:
omc_12 = [ 3.494229e-01 ; 2.881846e+00 ; -9.191323e-01 ];
Tc_12  = [ 1.324007e+02 ; -1.597497e+02 ; 4.564160e+02 ];
omc_error_12 = [ 1.717527e-03 ; 4.305316e-03 ; 6.023397e-03 ];
Tc_error_12  = [ 1.572432e+00 ; 1.958052e+00 ; 2.003527e+00 ];

%-- Image #13:
omc_13 = [ NaN ; NaN ; NaN ];
Tc_13  = [ NaN ; NaN ; NaN ];
omc_error_13 = [ NaN ; NaN ; NaN ];
Tc_error_13  = [ NaN ; NaN ; NaN ];

%-- Image #14:
omc_14 = [ -1.618160e+00 ; -2.559230e+00 ; 4.015716e-01 ];
Tc_14  = [ -9.076244e+00 ; -1.724781e+02 ; 4.517484e+02 ];
omc_error_14 = [ 2.827399e-03 ; 3.649389e-03 ; 6.563436e-03 ];
Tc_error_14  = [ 1.545882e+00 ; 1.904636e+00 ; 2.092199e+00 ];

%-- Image #15:
omc_15 = [ 1.862924e+00 ; 2.486343e+00 ; -3.559656e-01 ];
Tc_15  = [ -2.602009e+01 ; -1.567143e+02 ; 5.072516e+02 ];
omc_error_15 = [ 3.013905e-03 ; 4.051494e-03 ; 7.893506e-03 ];
Tc_error_15  = [ 1.724020e+00 ; 2.141544e+00 ; 2.350153e+00 ];

%-- Image #16:
omc_16 = [ 1.706953e+00 ; 2.276191e+00 ; -4.981129e-01 ];
Tc_16  = [ -2.427914e+01 ; -1.374359e+02 ; 5.697458e+02 ];
omc_error_16 = [ 2.886761e-03 ; 3.909602e-03 ; 6.682437e-03 ];
Tc_error_16  = [ 1.920264e+00 ; 2.409557e+00 ; 2.553288e+00 ];

%-- Image #17:
omc_17 = [ 1.586410e+00 ; 1.919322e+00 ; -8.777828e-01 ];
Tc_17  = [ -5.805119e+01 ; -8.020998e+01 ; 5.938839e+02 ];
omc_error_17 = [ 2.730232e-03 ; 3.931790e-03 ; 5.005515e-03 ];
Tc_error_17  = [ 1.988700e+00 ; 2.547206e+00 ; 2.275957e+00 ];

%-- Image #18:
omc_18 = [ 1.569240e+00 ; 1.954492e+00 ; -8.429097e-01 ];
Tc_18  = [ -5.221231e+01 ; -1.059217e+02 ; 5.532383e+02 ];
omc_error_18 = [ 2.678777e-03 ; 3.924525e-03 ; 4.973668e-03 ];
Tc_error_18  = [ 1.859787e+00 ; 2.369494e+00 ; 2.143771e+00 ];

%-- Image #19:
omc_19 = [ 1.291973e+00 ; 2.315801e+00 ; -8.350339e-01 ];
Tc_19  = [ 7.448509e+00 ; -1.184040e+02 ; 5.827690e+02 ];
omc_error_19 = [ 2.200281e-03 ; 4.160542e-03 ; 5.532396e-03 ];
Tc_error_19  = [ 1.953104e+00 ; 2.489540e+00 ; 2.356524e+00 ];

%-- Image #20:
omc_20 = [ 7.313821e-01 ; 2.618186e+00 ; -9.973776e-01 ];
Tc_20  = [ 8.970461e+01 ; -9.617679e+01 ; 5.930947e+02 ];
omc_error_20 = [ 1.766252e-03 ; 4.469927e-03 ; 5.723644e-03 ];
Tc_error_20  = [ 1.984824e+00 ; 2.548672e+00 ; 2.375492e+00 ];

