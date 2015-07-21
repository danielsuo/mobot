% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 574.813347086329941 ; 574.589317025302762 ];

%-- Principal point:
cc = [ 313.970783795772718 ; 240.882613180592386 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.057318054016189 ; 0.083568297462185 ; 0.002621036341615 ; -0.000928638373894 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 4.804295373777029 ; 4.405451221524971 ];

%-- Principal point uncertainty:
cc_error = [ 3.869748483231403 ; 5.038797644833188 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.016913816576662 ; 0.059213993965262 ; 0.001923190765858 ; 0.002194296728497 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 488;


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
omc_1 = [ 8.578771e-01 ; 2.413588e+00 ; -5.233736e-01 ];
Tc_1  = [ 3.838789e+01 ; -1.542236e+02 ; 5.201318e+02 ];
omc_error_1 = [ 4.437794e-03 ; 8.337745e-03 ; 1.049464e-02 ];
Tc_error_1  = [ 3.535793e+00 ; 4.495431e+00 ; 4.288214e+00 ];

%-- Image #2:
omc_2 = [ 7.779557e-01 ; 2.781330e+00 ; -5.396841e-01 ];
Tc_2  = [ 6.716907e+01 ; -1.490078e+02 ; 5.032147e+02 ];
omc_error_2 = [ 2.938445e-03 ; 8.836292e-03 ; 1.265445e-02 ];
Tc_error_2  = [ 3.412603e+00 ; 4.301266e+00 ; 4.479721e+00 ];

%-- Image #3:
omc_3 = [ -6.842780e-01 ; -2.914089e+00 ; 5.976279e-01 ];
Tc_3  = [ 9.241459e+01 ; -1.438471e+02 ; 4.950273e+02 ];
omc_error_3 = [ 4.279699e-03 ; 8.556439e-03 ; 1.368002e-02 ];
Tc_error_3  = [ 3.372253e+00 ; 4.242679e+00 ; 4.519629e+00 ];

%-- Image #4:
omc_4 = [ -6.584066e-01 ; -2.811453e+00 ; 6.028421e-01 ];
Tc_4  = [ 3.507928e+01 ; -1.343301e+02 ; 4.967658e+02 ];
omc_error_4 = [ 4.458044e-03 ; 8.392583e-03 ; 1.266079e-02 ];
Tc_error_4  = [ 3.371010e+00 ; 4.261455e+00 ; 4.563263e+00 ];

%-- Image #5:
omc_5 = [ -6.127478e-01 ; -2.689096e+00 ; 5.153155e-01 ];
Tc_5  = [ 1.277156e+01 ; -1.523207e+02 ; 4.381551e+02 ];
omc_error_5 = [ 4.603018e-03 ; 8.041607e-03 ; 1.129322e-02 ];
Tc_error_5  = [ 3.000654e+00 ; 3.758014e+00 ; 4.130363e+00 ];

%-- Image #6:
omc_6 = [ -6.203517e-01 ; -2.669809e+00 ; 4.969081e-01 ];
Tc_6  = [ -5.525184e+00 ; -1.370034e+02 ; 4.021315e+02 ];
omc_error_6 = [ 4.517926e-03 ; 7.840195e-03 ; 1.097182e-02 ];
Tc_error_6  = [ 2.741587e+00 ; 3.455087e+00 ; 3.815963e+00 ];

%-- Image #7:
omc_7 = [ -6.058628e-01 ; -2.830377e+00 ; 5.815482e-01 ];
Tc_7  = [ 2.287067e+01 ; -1.094110e+02 ; 4.671226e+02 ];
omc_error_7 = [ 4.056853e-03 ; 8.277651e-03 ; 1.250813e-02 ];
Tc_error_7  = [ 3.151352e+00 ; 4.020692e+00 ; 4.294533e+00 ];

%-- Image #8:
omc_8 = [ -5.870884e-01 ; -3.002556e+00 ; 6.800537e-01 ];
Tc_8  = [ 3.984421e+01 ; -1.081661e+02 ; 4.935198e+02 ];
omc_error_8 = [ 4.017628e-03 ; 8.204209e-03 ; 1.377715e-02 ];
Tc_error_8  = [ 3.328049e+00 ; 4.235039e+00 ; 4.417120e+00 ];

%-- Image #9:
omc_9 = [ 5.160489e-01 ; 3.082461e+00 ; 8.012546e-02 ];
Tc_9  = [ 6.749337e+01 ; -1.434234e+02 ; 3.770257e+02 ];
omc_error_9 = [ 2.446758e-03 ; 8.479191e-03 ; 1.322956e-02 ];
Tc_error_9  = [ 2.608148e+00 ; 3.362309e+00 ; 3.423149e+00 ];

%-- Image #10:
omc_10 = [ 4.075283e-01 ; 3.048539e+00 ; -3.245352e-01 ];
Tc_10  = [ 8.955099e+01 ; -1.340808e+02 ; 4.195206e+02 ];
omc_error_10 = [ 2.024906e-03 ; 8.604274e-03 ; 1.291632e-02 ];
Tc_error_10  = [ 2.859680e+00 ; 3.585701e+00 ; 3.861249e+00 ];

%-- Image #11:
omc_11 = [ 2.744041e-01 ; 2.873595e+00 ; -9.923259e-01 ];
Tc_11  = [ 9.954878e+01 ; -1.194622e+02 ; 4.640523e+02 ];
omc_error_11 = [ 3.439311e-03 ; 8.682277e-03 ; 1.203151e-02 ];
Tc_error_11  = [ 3.161550e+00 ; 4.005830e+00 ; 3.874724e+00 ];

%-- Image #12:
omc_12 = [ 3.555987e-01 ; 2.841032e+00 ; -9.629644e-01 ];
Tc_12  = [ 9.841913e+01 ; -1.530611e+02 ; 4.438455e+02 ];
omc_error_12 = [ 3.489320e-03 ; 8.741063e-03 ; 1.179619e-02 ];
Tc_error_12  = [ 3.067134e+00 ; 3.820469e+00 ; 3.762392e+00 ];

%-- Image #13:
omc_13 = [ NaN ; NaN ; NaN ];
Tc_13  = [ NaN ; NaN ; NaN ];
omc_error_13 = [ NaN ; NaN ; NaN ];
Tc_error_13  = [ NaN ; NaN ; NaN ];

%-- Image #14:
omc_14 = [ -1.617898e+00 ; -2.553717e+00 ; 4.256498e-01 ];
Tc_14  = [ -4.031862e+01 ; -1.692967e+02 ; 4.312112e+02 ];
omc_error_14 = [ 5.684811e-03 ; 6.853564e-03 ; 1.243164e-02 ];
Tc_error_14  = [ 2.957555e+00 ; 3.676968e+00 ; 3.955945e+00 ];

%-- Image #15:
omc_15 = [ 1.868288e+00 ; 2.466520e+00 ; -3.757149e-01 ];
Tc_15  = [ -6.056559e+01 ; -1.510313e+02 ; 4.897800e+02 ];
omc_error_15 = [ 5.568941e-03 ; 8.092796e-03 ; 1.506299e-02 ];
Tc_error_15  = [ 3.334952e+00 ; 4.190145e+00 ; 4.493895e+00 ];

%-- Image #16:
omc_16 = [ 1.709430e+00 ; 2.267631e+00 ; -5.065611e-01 ];
Tc_16  = [ -5.727290e+01 ; -1.317715e+02 ; 5.492325e+02 ];
omc_error_16 = [ 5.543891e-03 ; 8.003644e-03 ; 1.335232e-02 ];
Tc_error_16  = [ 3.714153e+00 ; 4.704281e+00 ; 4.892511e+00 ];

%-- Image #17:
omc_17 = [ 1.584818e+00 ; 1.901606e+00 ; -8.919149e-01 ];
Tc_17  = [ -9.147729e+01 ; -7.293098e+01 ; 5.749327e+02 ];
omc_error_17 = [ 5.273977e-03 ; 8.063833e-03 ; 9.936371e-03 ];
Tc_error_17  = [ 3.868180e+00 ; 5.008313e+00 ; 4.385939e+00 ];

%-- Image #18:
omc_18 = [ 1.568214e+00 ; 1.937635e+00 ; -8.587137e-01 ];
Tc_18  = [ -8.552887e+01 ; -9.988906e+01 ; 5.340799e+02 ];
omc_error_18 = [ 5.139924e-03 ; 8.058187e-03 ; 9.889047e-03 ];
Tc_error_18  = [ 3.607642e+00 ; 4.645080e+00 ; 4.113539e+00 ];

%-- Image #19:
omc_19 = [ 1.298847e+00 ; 2.293219e+00 ; -8.572262e-01 ];
Tc_19  = [ -2.685597e+01 ; -1.114481e+02 ; 5.635457e+02 ];
omc_error_19 = [ 4.207198e-03 ; 8.500160e-03 ; 1.094320e-02 ];
Tc_error_19  = [ 3.792491e+00 ; 4.873269e+00 ; 4.497088e+00 ];

%-- Image #20:
omc_20 = [ 7.422369e-01 ; 2.602201e+00 ; -1.021473e+00 ];
Tc_20  = [ 5.556101e+01 ; -9.012823e+01 ; 5.747449e+02 ];
omc_error_20 = [ 3.410898e-03 ; 9.100451e-03 ; 1.137486e-02 ];
Tc_error_20  = [ 3.862185e+00 ; 4.985739e+00 ; 4.514520e+00 ];

