% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 563.316457921538586 ; 570.684770040221338 ];

%-- Principal point:
cc = [ 323.352113524533365 ; 235.125227152132283 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.002441902679782 ; -0.032632548797317 ; -0.001316760701295 ; -0.000233356562565 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 0.917990292501679 ; 0.898080088240865 ];

%-- Principal point uncertainty:
cc_error = [ 0.936910095661388 ; 1.244027589182474 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.007365158562689 ; 0.045341647072814 ; 0.000750126278863 ; 0.000620963923184 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 488;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 21;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.997996e-01 ; 2.645209e+00 ; 8.461229e-01 ];
Tc_1  = [ 1.673053e+02 ; -1.635955e+02 ; 5.303529e+02 ];
omc_error_1 = [ 1.152526e-03 ; 2.058998e-03 ; 3.079738e-03 ];
Tc_error_1  = [ 9.002822e-01 ; 1.193698e+00 ; 1.040433e+00 ];

%-- Image #2:
omc_2 = [ 8.122439e-02 ; 2.934707e+00 ; 7.917318e-01 ];
Tc_2  = [ 1.608968e+02 ; -1.213082e+02 ; 5.541032e+02 ];
omc_error_2 = [ 9.676005e-04 ; 2.244448e-03 ; 3.640564e-03 ];
Tc_error_2  = [ 9.310200e-01 ; 1.242367e+00 ; 1.078908e+00 ];

%-- Image #3:
omc_3 = [ 3.525311e-01 ; -2.297442e+00 ; -5.312448e-01 ];
Tc_3  = [ 1.561432e+02 ; -4.319916e+01 ; 5.327881e+02 ];
omc_error_3 = [ 1.231856e-03 ; 1.937129e-03 ; 2.815066e-03 ];
Tc_error_3  = [ 9.092247e-01 ; 1.190913e+00 ; 1.084398e+00 ];

%-- Image #4:
omc_4 = [ 2.786455e-01 ; -2.783260e+00 ; -5.988460e-01 ];
Tc_4  = [ 9.218800e+01 ; -7.168628e+01 ; 4.321184e+02 ];
omc_error_4 = [ 9.284374e-04 ; 2.072107e-03 ; 3.342116e-03 ];
Tc_error_4  = [ 7.272433e-01 ; 9.515966e-01 ; 8.312524e-01 ];

%-- Image #5:
omc_5 = [ 1.407020e-01 ; -3.027555e+00 ; -6.677443e-01 ];
Tc_5  = [ 9.530702e+01 ; -8.089553e+01 ; 4.763098e+02 ];
omc_error_5 = [ 8.524080e-04 ; 2.188478e-03 ; 3.742585e-03 ];
Tc_error_5  = [ 7.982926e-01 ; 1.049168e+00 ; 8.905022e-01 ];

%-- Image #6:
omc_6 = [ 9.436892e-03 ; 2.908229e+00 ; 6.474355e-01 ];
Tc_6  = [ 8.354190e+01 ; -9.567444e+01 ; 4.875125e+02 ];
omc_error_6 = [ 8.051464e-04 ; 2.111449e-03 ; 3.575919e-03 ];
Tc_error_6  = [ 8.153717e-01 ; 1.072411e+00 ; 8.698317e-01 ];

%-- Image #7:
omc_7 = [ 1.042238e-01 ; 2.775464e+00 ; 6.129372e-01 ];
Tc_7  = [ 7.830145e+01 ; -1.126938e+02 ; 4.949127e+02 ];
omc_error_7 = [ 8.702540e-04 ; 2.036165e-03 ; 3.340286e-03 ];
Tc_error_7  = [ 8.278072e-01 ; 1.088994e+00 ; 8.473164e-01 ];

%-- Image #8:
omc_8 = [ 9.161245e-02 ; 2.710597e+00 ; 6.359470e-01 ];
Tc_8  = [ 1.193782e+02 ; -1.239420e+02 ; 4.660518e+02 ];
omc_error_8 = [ 9.419589e-04 ; 1.967667e-03 ; 3.157309e-03 ];
Tc_error_8  = [ 7.824962e-01 ; 1.033696e+00 ; 8.161086e-01 ];

%-- Image #9:
omc_9 = [ -2.788958e-02 ; 2.876129e+00 ; 6.217292e-02 ];
Tc_9  = [ 1.477011e+02 ; -1.090100e+02 ; 5.991383e+02 ];
omc_error_9 = [ 7.465798e-04 ; 2.472407e-03 ; 3.929635e-03 ];
Tc_error_9  = [ 1.000050e+00 ; 1.320611e+00 ; 9.956635e-01 ];

%-- Image #10:
omc_10 = [ -1.524101e-02 ; 2.732768e+00 ; 1.318667e-01 ];
Tc_10  = [ 1.118370e+02 ; -1.595514e+02 ; 5.852854e+02 ];
omc_error_10 = [ 7.916656e-04 ; 2.173426e-03 ; 3.432479e-03 ];
Tc_error_10  = [ 9.859792e-01 ; 1.282143e+00 ; 9.502371e-01 ];

%-- Image #11:
omc_11 = [ 4.246385e-02 ; 2.589030e+00 ; 7.801018e-02 ];
Tc_11  = [ 4.676650e+01 ; -1.465448e+02 ; 5.777244e+02 ];
omc_error_11 = [ 8.064105e-04 ; 1.983772e-03 ; 3.094384e-03 ];
Tc_error_11  = [ 9.687020e-01 ; 1.255939e+00 ; 8.462471e-01 ];

%-- Image #12:
omc_12 = [ -5.344746e-02 ; 3.079884e+00 ; 4.542802e-01 ];
Tc_12  = [ 1.050599e+02 ; -1.016931e+02 ; 5.035930e+02 ];
omc_error_12 = [ 6.532234e-04 ; 2.371203e-03 ; 4.041852e-03 ];
Tc_error_12  = [ 8.454717e-01 ; 1.113793e+00 ; 9.407528e-01 ];

%-- Image #13:
omc_13 = [ 4.992464e-02 ; -3.105024e+00 ; -2.902876e-01 ];
Tc_13  = [ 1.372505e+02 ; -1.908445e+02 ; 5.651753e+02 ];
omc_error_13 = [ 6.449791e-04 ; 3.390803e-03 ; 4.965738e-03 ];
Tc_error_13  = [ 9.715397e-01 ; 1.271536e+00 ; 1.147496e+00 ];

%-- Image #14:
omc_14 = [ 4.120724e-03 ; -3.014771e+00 ; 2.473526e-01 ];
Tc_14  = [ 1.231244e+02 ; -1.629079e+02 ; 5.498001e+02 ];
omc_error_14 = [ 5.801060e-04 ; 2.562645e-03 ; 4.071121e-03 ];
Tc_error_14  = [ 9.314425e-01 ; 1.206870e+00 ; 9.528686e-01 ];

%-- Image #15:
omc_15 = [ -6.181779e-02 ; -3.022245e+00 ; 3.279743e-01 ];
Tc_15  = [ 9.413632e+01 ; -1.799521e+02 ; 5.677899e+02 ];
omc_error_15 = [ 6.713938e-04 ; 2.426209e-03 ; 4.011493e-03 ];
Tc_error_15  = [ 9.631577e-01 ; 1.240221e+00 ; 9.503126e-01 ];

%-- Image #16:
omc_16 = [ 1.513367e-02 ; 3.037946e+00 ; 6.886276e-01 ];
Tc_16  = [ 1.013670e+02 ; -1.127934e+02 ; 5.491092e+02 ];
omc_error_16 = [ 8.323587e-04 ; 2.319819e-03 ; 3.900391e-03 ];
Tc_error_16  = [ 9.223889e-01 ; 1.216341e+00 ; 1.041291e+00 ];

%-- Image #17:
omc_17 = [ 5.655081e-01 ; -2.335489e+00 ; -5.241371e-01 ];
Tc_17  = [ 1.817811e+02 ; -2.300567e+01 ; 4.672794e+02 ];
omc_error_17 = [ 1.207522e-03 ; 1.877097e-03 ; 2.848065e-03 ];
Tc_error_17  = [ 8.096411e-01 ; 1.057830e+00 ; 9.807951e-01 ];

%-- Image #18:
omc_18 = [ 5.740560e-01 ; -2.362773e+00 ; -6.052447e-01 ];
Tc_18  = [ 1.365703e+02 ; -5.018501e+01 ; 3.657331e+02 ];
omc_error_18 = [ 1.166517e-03 ; 1.809140e-03 ; 2.734105e-03 ];
Tc_error_18  = [ 6.319231e-01 ; 8.258716e-01 ; 7.888815e-01 ];

%-- Image #19:
omc_19 = [ 4.930338e-01 ; -2.706677e+00 ; -5.479435e-01 ];
Tc_19  = [ 8.578342e+01 ; -6.911141e+00 ; 4.478073e+02 ];
omc_error_19 = [ 1.142704e-03 ; 2.007528e-03 ; 3.418712e-03 ];
Tc_error_19  = [ 7.476929e-01 ; 9.752571e-01 ; 8.487774e-01 ];

%-- Image #20:
omc_20 = [ 2.471480e-01 ; -2.835244e+00 ; -7.818564e-01 ];
Tc_20  = [ 1.198575e+02 ; -9.951210e+01 ; 4.276642e+02 ];
omc_error_20 = [ 8.973925e-04 ; 2.051206e-03 ; 3.301733e-03 ];
Tc_error_20  = [ 7.282624e-01 ; 9.590179e-01 ; 8.421858e-01 ];

%-- Image #21:
omc_21 = [ 8.608610e-02 ; 2.875428e+00 ; 7.867530e-02 ];
Tc_21  = [ 6.340604e+01 ; -1.554533e+02 ; 5.437756e+02 ];
omc_error_21 = [ 5.672366e-04 ; 2.511409e-03 ; 3.755459e-03 ];
Tc_error_21  = [ 9.150615e-01 ; 1.189097e+00 ; 9.199041e-01 ];

