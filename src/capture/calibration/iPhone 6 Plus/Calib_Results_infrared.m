% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 567.475712286176190 ; 563.474072159062416 ];

%-- Principal point:
cc = [ 319.212006789408747 ; 243.575129433152739 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.062648887464614 ; 0.048983559168708 ; -0.000574495439515 ; -0.000565695970238 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 2.060732606180453 ; 2.150892218159688 ];

%-- Principal point uncertainty:
cc_error = [ 2.878877427482895 ; 2.571185414156203 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.011565402086985 ; 0.034771113971424 ; 0.001149126354059 ; 0.001582594208753 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 488;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 114;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.889977e+00 ; 1.946882e+00 ; 5.458099e-01 ];
Tc_1  = [ -1.245546e+02 ; -1.057402e+02 ; 2.841311e+02 ];
omc_error_1 = [ 4.742437e-03 ; 4.110846e-03 ; 7.298764e-03 ];
Tc_error_1  = [ 1.540661e+00 ; 1.353580e+00 ; 1.649062e+00 ];

%-- Image #2:
omc_2 = [ NaN ; NaN ; NaN ];
Tc_2  = [ NaN ; NaN ; NaN ];
omc_error_2 = [ NaN ; NaN ; NaN ];
Tc_error_2  = [ NaN ; NaN ; NaN ];

%-- Image #3:
omc_3 = [ 2.194834e+00 ; 2.225238e+00 ; -8.430735e-02 ];
Tc_3  = [ -1.544758e+02 ; -1.049491e+02 ; 3.586516e+02 ];
omc_error_3 = [ 4.282297e-03 ; 4.682544e-03 ; 9.673799e-03 ];
Tc_error_3  = [ 1.861584e+00 ; 1.671589e+00 ; 1.730977e+00 ];

%-- Image #4:
omc_4 = [ NaN ; NaN ; NaN ];
Tc_4  = [ NaN ; NaN ; NaN ];
omc_error_4 = [ NaN ; NaN ; NaN ];
Tc_error_4  = [ NaN ; NaN ; NaN ];

%-- Image #5:
omc_5 = [ NaN ; NaN ; NaN ];
Tc_5  = [ NaN ; NaN ; NaN ];
omc_error_5 = [ NaN ; NaN ; NaN ];
Tc_error_5  = [ NaN ; NaN ; NaN ];

%-- Image #6:
omc_6 = [ NaN ; NaN ; NaN ];
Tc_6  = [ NaN ; NaN ; NaN ];
omc_error_6 = [ NaN ; NaN ; NaN ];
Tc_error_6  = [ NaN ; NaN ; NaN ];

%-- Image #7:
omc_7 = [ NaN ; NaN ; NaN ];
Tc_7  = [ NaN ; NaN ; NaN ];
omc_error_7 = [ NaN ; NaN ; NaN ];
Tc_error_7  = [ NaN ; NaN ; NaN ];

%-- Image #8:
omc_8 = [ NaN ; NaN ; NaN ];
Tc_8  = [ NaN ; NaN ; NaN ];
omc_error_8 = [ NaN ; NaN ; NaN ];
Tc_error_8  = [ NaN ; NaN ; NaN ];

%-- Image #9:
omc_9 = [ NaN ; NaN ; NaN ];
Tc_9  = [ NaN ; NaN ; NaN ];
omc_error_9 = [ NaN ; NaN ; NaN ];
Tc_error_9  = [ NaN ; NaN ; NaN ];

%-- Image #10:
omc_10 = [ NaN ; NaN ; NaN ];
Tc_10  = [ NaN ; NaN ; NaN ];
omc_error_10 = [ NaN ; NaN ; NaN ];
Tc_error_10  = [ NaN ; NaN ; NaN ];

%-- Image #11:
omc_11 = [ NaN ; NaN ; NaN ];
Tc_11  = [ NaN ; NaN ; NaN ];
omc_error_11 = [ NaN ; NaN ; NaN ];
Tc_error_11  = [ NaN ; NaN ; NaN ];

%-- Image #12:
omc_12 = [ NaN ; NaN ; NaN ];
Tc_12  = [ NaN ; NaN ; NaN ];
omc_error_12 = [ NaN ; NaN ; NaN ];
Tc_error_12  = [ NaN ; NaN ; NaN ];

%-- Image #13:
omc_13 = [ NaN ; NaN ; NaN ];
Tc_13  = [ NaN ; NaN ; NaN ];
omc_error_13 = [ NaN ; NaN ; NaN ];
Tc_error_13  = [ NaN ; NaN ; NaN ];

%-- Image #14:
omc_14 = [ NaN ; NaN ; NaN ];
Tc_14  = [ NaN ; NaN ; NaN ];
omc_error_14 = [ NaN ; NaN ; NaN ];
Tc_error_14  = [ NaN ; NaN ; NaN ];

%-- Image #15:
omc_15 = [ NaN ; NaN ; NaN ];
Tc_15  = [ NaN ; NaN ; NaN ];
omc_error_15 = [ NaN ; NaN ; NaN ];
Tc_error_15  = [ NaN ; NaN ; NaN ];

%-- Image #16:
omc_16 = [ -1.930347e+00 ; -1.921834e+00 ; 6.595042e-01 ];
Tc_16  = [ -1.196758e+02 ; -9.557899e+01 ; 4.484705e+02 ];
omc_error_16 = [ 4.546430e-03 ; 3.847070e-03 ; 7.629547e-03 ];
Tc_error_16  = [ 2.282616e+00 ; 2.027759e+00 ; 1.671495e+00 ];

%-- Image #17:
omc_17 = [ NaN ; NaN ; NaN ];
Tc_17  = [ NaN ; NaN ; NaN ];
omc_error_17 = [ NaN ; NaN ; NaN ];
Tc_error_17  = [ NaN ; NaN ; NaN ];

%-- Image #18:
omc_18 = [ NaN ; NaN ; NaN ];
Tc_18  = [ NaN ; NaN ; NaN ];
omc_error_18 = [ NaN ; NaN ; NaN ];
Tc_error_18  = [ NaN ; NaN ; NaN ];

%-- Image #19:
omc_19 = [ NaN ; NaN ; NaN ];
Tc_19  = [ NaN ; NaN ; NaN ];
omc_error_19 = [ NaN ; NaN ; NaN ];
Tc_error_19  = [ NaN ; NaN ; NaN ];

%-- Image #20:
omc_20 = [ 2.095396e+00 ; 2.072036e+00 ; -2.826611e-01 ];
Tc_20  = [ -1.460498e+02 ; -9.777264e+01 ; 3.855422e+02 ];
omc_error_20 = [ 4.065510e-03 ; 4.738201e-03 ; 9.024752e-03 ];
Tc_error_20  = [ 1.985194e+00 ; 1.762554e+00 ; 1.805336e+00 ];

%-- Image #21:
omc_21 = [ NaN ; NaN ; NaN ];
Tc_21  = [ NaN ; NaN ; NaN ];
omc_error_21 = [ NaN ; NaN ; NaN ];
Tc_error_21  = [ NaN ; NaN ; NaN ];

%-- Image #22:
omc_22 = [ NaN ; NaN ; NaN ];
Tc_22  = [ NaN ; NaN ; NaN ];
omc_error_22 = [ NaN ; NaN ; NaN ];
Tc_error_22  = [ NaN ; NaN ; NaN ];

%-- Image #23:
omc_23 = [ NaN ; NaN ; NaN ];
Tc_23  = [ NaN ; NaN ; NaN ];
omc_error_23 = [ NaN ; NaN ; NaN ];
Tc_error_23  = [ NaN ; NaN ; NaN ];

%-- Image #24:
omc_24 = [ NaN ; NaN ; NaN ];
Tc_24  = [ NaN ; NaN ; NaN ];
omc_error_24 = [ NaN ; NaN ; NaN ];
Tc_error_24  = [ NaN ; NaN ; NaN ];

%-- Image #25:
omc_25 = [ NaN ; NaN ; NaN ];
Tc_25  = [ NaN ; NaN ; NaN ];
omc_error_25 = [ NaN ; NaN ; NaN ];
Tc_error_25  = [ NaN ; NaN ; NaN ];

%-- Image #26:
omc_26 = [ NaN ; NaN ; NaN ];
Tc_26  = [ NaN ; NaN ; NaN ];
omc_error_26 = [ NaN ; NaN ; NaN ];
Tc_error_26  = [ NaN ; NaN ; NaN ];

%-- Image #27:
omc_27 = [ NaN ; NaN ; NaN ];
Tc_27  = [ NaN ; NaN ; NaN ];
omc_error_27 = [ NaN ; NaN ; NaN ];
Tc_error_27  = [ NaN ; NaN ; NaN ];

%-- Image #28:
omc_28 = [ NaN ; NaN ; NaN ];
Tc_28  = [ NaN ; NaN ; NaN ];
omc_error_28 = [ NaN ; NaN ; NaN ];
Tc_error_28  = [ NaN ; NaN ; NaN ];

%-- Image #29:
omc_29 = [ NaN ; NaN ; NaN ];
Tc_29  = [ NaN ; NaN ; NaN ];
omc_error_29 = [ NaN ; NaN ; NaN ];
Tc_error_29  = [ NaN ; NaN ; NaN ];

%-- Image #30:
omc_30 = [ NaN ; NaN ; NaN ];
Tc_30  = [ NaN ; NaN ; NaN ];
omc_error_30 = [ NaN ; NaN ; NaN ];
Tc_error_30  = [ NaN ; NaN ; NaN ];

%-- Image #31:
omc_31 = [ NaN ; NaN ; NaN ];
Tc_31  = [ NaN ; NaN ; NaN ];
omc_error_31 = [ NaN ; NaN ; NaN ];
Tc_error_31  = [ NaN ; NaN ; NaN ];

%-- Image #32:
omc_32 = [ NaN ; NaN ; NaN ];
Tc_32  = [ NaN ; NaN ; NaN ];
omc_error_32 = [ NaN ; NaN ; NaN ];
Tc_error_32  = [ NaN ; NaN ; NaN ];

%-- Image #33:
omc_33 = [ NaN ; NaN ; NaN ];
Tc_33  = [ NaN ; NaN ; NaN ];
omc_error_33 = [ NaN ; NaN ; NaN ];
Tc_error_33  = [ NaN ; NaN ; NaN ];

%-- Image #34:
omc_34 = [ NaN ; NaN ; NaN ];
Tc_34  = [ NaN ; NaN ; NaN ];
omc_error_34 = [ NaN ; NaN ; NaN ];
Tc_error_34  = [ NaN ; NaN ; NaN ];

%-- Image #35:
omc_35 = [ NaN ; NaN ; NaN ];
Tc_35  = [ NaN ; NaN ; NaN ];
omc_error_35 = [ NaN ; NaN ; NaN ];
Tc_error_35  = [ NaN ; NaN ; NaN ];

%-- Image #36:
omc_36 = [ NaN ; NaN ; NaN ];
Tc_36  = [ NaN ; NaN ; NaN ];
omc_error_36 = [ NaN ; NaN ; NaN ];
Tc_error_36  = [ NaN ; NaN ; NaN ];

%-- Image #37:
omc_37 = [ NaN ; NaN ; NaN ];
Tc_37  = [ NaN ; NaN ; NaN ];
omc_error_37 = [ NaN ; NaN ; NaN ];
Tc_error_37  = [ NaN ; NaN ; NaN ];

%-- Image #38:
omc_38 = [ NaN ; NaN ; NaN ];
Tc_38  = [ NaN ; NaN ; NaN ];
omc_error_38 = [ NaN ; NaN ; NaN ];
Tc_error_38  = [ NaN ; NaN ; NaN ];

%-- Image #39:
omc_39 = [ NaN ; NaN ; NaN ];
Tc_39  = [ NaN ; NaN ; NaN ];
omc_error_39 = [ NaN ; NaN ; NaN ];
Tc_error_39  = [ NaN ; NaN ; NaN ];

%-- Image #40:
omc_40 = [ NaN ; NaN ; NaN ];
Tc_40  = [ NaN ; NaN ; NaN ];
omc_error_40 = [ NaN ; NaN ; NaN ];
Tc_error_40  = [ NaN ; NaN ; NaN ];

%-- Image #41:
omc_41 = [ NaN ; NaN ; NaN ];
Tc_41  = [ NaN ; NaN ; NaN ];
omc_error_41 = [ NaN ; NaN ; NaN ];
Tc_error_41  = [ NaN ; NaN ; NaN ];

%-- Image #42:
omc_42 = [ -2.040938e+00 ; -2.099879e+00 ; -3.418169e-01 ];
Tc_42  = [ -1.564156e+02 ; -8.875062e+01 ; 3.223367e+02 ];
omc_error_42 = [ 3.949077e-03 ; 4.669547e-03 ; 8.781233e-03 ];
Tc_error_42  = [ 1.684872e+00 ; 1.558093e+00 ; 1.686793e+00 ];

%-- Image #43:
omc_43 = [ 2.134035e+00 ; 1.414122e+00 ; -2.542525e-01 ];
Tc_43  = [ -1.963639e+02 ; -7.736172e+01 ; 3.715803e+02 ];
omc_error_43 = [ 4.274332e-03 ; 4.424646e-03 ; 7.147554e-03 ];
Tc_error_43  = [ 1.923533e+00 ; 1.744718e+00 ; 1.825731e+00 ];

%-- Image #44:
omc_44 = [ 2.322682e+00 ; 1.101828e+00 ; -1.763581e-01 ];
Tc_44  = [ -1.700522e+02 ; -3.772139e+01 ; 3.525067e+02 ];
omc_error_44 = [ 4.774363e-03 ; 3.670737e-03 ; 7.414143e-03 ];
Tc_error_44  = [ 1.823724e+00 ; 1.638982e+00 ; 1.683109e+00 ];

%-- Image #45:
omc_45 = [ NaN ; NaN ; NaN ];
Tc_45  = [ NaN ; NaN ; NaN ];
omc_error_45 = [ NaN ; NaN ; NaN ];
Tc_error_45  = [ NaN ; NaN ; NaN ];

%-- Image #46:
omc_46 = [ NaN ; NaN ; NaN ];
Tc_46  = [ NaN ; NaN ; NaN ];
omc_error_46 = [ NaN ; NaN ; NaN ];
Tc_error_46  = [ NaN ; NaN ; NaN ];

%-- Image #47:
omc_47 = [ NaN ; NaN ; NaN ];
Tc_47  = [ NaN ; NaN ; NaN ];
omc_error_47 = [ NaN ; NaN ; NaN ];
Tc_error_47  = [ NaN ; NaN ; NaN ];

%-- Image #48:
omc_48 = [ NaN ; NaN ; NaN ];
Tc_48  = [ NaN ; NaN ; NaN ];
omc_error_48 = [ NaN ; NaN ; NaN ];
Tc_error_48  = [ NaN ; NaN ; NaN ];

%-- Image #49:
omc_49 = [ NaN ; NaN ; NaN ];
Tc_49  = [ NaN ; NaN ; NaN ];
omc_error_49 = [ NaN ; NaN ; NaN ];
Tc_error_49  = [ NaN ; NaN ; NaN ];

%-- Image #50:
omc_50 = [ NaN ; NaN ; NaN ];
Tc_50  = [ NaN ; NaN ; NaN ];
omc_error_50 = [ NaN ; NaN ; NaN ];
Tc_error_50  = [ NaN ; NaN ; NaN ];

%-- Image #51:
omc_51 = [ NaN ; NaN ; NaN ];
Tc_51  = [ NaN ; NaN ; NaN ];
omc_error_51 = [ NaN ; NaN ; NaN ];
Tc_error_51  = [ NaN ; NaN ; NaN ];

%-- Image #52:
omc_52 = [ NaN ; NaN ; NaN ];
Tc_52  = [ NaN ; NaN ; NaN ];
omc_error_52 = [ NaN ; NaN ; NaN ];
Tc_error_52  = [ NaN ; NaN ; NaN ];

%-- Image #53:
omc_53 = [ 2.446813e+00 ; 4.106164e-01 ; 3.785909e-02 ];
Tc_53  = [ -1.185834e+02 ; 3.316669e+01 ; 3.518888e+02 ];
omc_error_53 = [ 5.241247e-03 ; 2.914537e-03 ; 7.747394e-03 ];
Tc_error_53  = [ 1.811710e+00 ; 1.647067e+00 ; 1.665413e+00 ];

%-- Image #54:
omc_54 = [ NaN ; NaN ; NaN ];
Tc_54  = [ NaN ; NaN ; NaN ];
omc_error_54 = [ NaN ; NaN ; NaN ];
Tc_error_54  = [ NaN ; NaN ; NaN ];

%-- Image #55:
omc_55 = [ NaN ; NaN ; NaN ];
Tc_55  = [ NaN ; NaN ; NaN ];
omc_error_55 = [ NaN ; NaN ; NaN ];
Tc_error_55  = [ NaN ; NaN ; NaN ];

%-- Image #56:
omc_56 = [ 2.432520e+00 ; 2.409537e-02 ; 1.486696e-01 ];
Tc_56  = [ -1.150199e+02 ; 8.986549e+01 ; 3.273363e+02 ];
omc_error_56 = [ 5.258175e-03 ; 2.745203e-03 ; 7.554107e-03 ];
Tc_error_56  = [ 1.715498e+00 ; 1.585488e+00 ; 1.605301e+00 ];

%-- Image #57:
omc_57 = [ NaN ; NaN ; NaN ];
Tc_57  = [ NaN ; NaN ; NaN ];
omc_error_57 = [ NaN ; NaN ; NaN ];
Tc_error_57  = [ NaN ; NaN ; NaN ];

%-- Image #58:
omc_58 = [ NaN ; NaN ; NaN ];
Tc_58  = [ NaN ; NaN ; NaN ];
omc_error_58 = [ NaN ; NaN ; NaN ];
Tc_error_58  = [ NaN ; NaN ; NaN ];

%-- Image #59:
omc_59 = [ NaN ; NaN ; NaN ];
Tc_59  = [ NaN ; NaN ; NaN ];
omc_error_59 = [ NaN ; NaN ; NaN ];
Tc_error_59  = [ NaN ; NaN ; NaN ];

%-- Image #60:
omc_60 = [ 2.403241e+00 ; -3.115519e-01 ; 2.317401e-01 ];
Tc_60  = [ -8.209143e+01 ; 1.051493e+02 ; 3.242840e+02 ];
omc_error_60 = [ 5.267853e-03 ; 2.922902e-03 ; 7.464359e-03 ];
Tc_error_60  = [ 1.699342e+00 ; 1.577690e+00 ; 1.594022e+00 ];

%-- Image #61:
omc_61 = [ NaN ; NaN ; NaN ];
Tc_61  = [ NaN ; NaN ; NaN ];
omc_error_61 = [ NaN ; NaN ; NaN ];
Tc_error_61  = [ NaN ; NaN ; NaN ];

%-- Image #62:
omc_62 = [ NaN ; NaN ; NaN ];
Tc_62  = [ NaN ; NaN ; NaN ];
omc_error_62 = [ NaN ; NaN ; NaN ];
Tc_error_62  = [ NaN ; NaN ; NaN ];

%-- Image #63:
omc_63 = [ NaN ; NaN ; NaN ];
Tc_63  = [ NaN ; NaN ; NaN ];
omc_error_63 = [ NaN ; NaN ; NaN ];
Tc_error_63  = [ NaN ; NaN ; NaN ];

%-- Image #64:
omc_64 = [ 2.366244e+00 ; -7.449612e-01 ; 3.709559e-01 ];
Tc_64  = [ -3.627634e+01 ; 1.007361e+02 ; 2.985418e+02 ];
omc_error_64 = [ 5.087962e-03 ; 3.307610e-03 ; 7.359637e-03 ];
Tc_error_64  = [ 1.551671e+00 ; 1.443254e+00 ; 1.436011e+00 ];

%-- Image #65:
omc_65 = [ NaN ; NaN ; NaN ];
Tc_65  = [ NaN ; NaN ; NaN ];
omc_error_65 = [ NaN ; NaN ; NaN ];
Tc_error_65  = [ NaN ; NaN ; NaN ];

%-- Image #66:
omc_66 = [ NaN ; NaN ; NaN ];
Tc_66  = [ NaN ; NaN ; NaN ];
omc_error_66 = [ NaN ; NaN ; NaN ];
Tc_error_66  = [ NaN ; NaN ; NaN ];

%-- Image #67:
omc_67 = [ NaN ; NaN ; NaN ];
Tc_67  = [ NaN ; NaN ; NaN ];
omc_error_67 = [ NaN ; NaN ; NaN ];
Tc_error_67  = [ NaN ; NaN ; NaN ];

%-- Image #68:
omc_68 = [ NaN ; NaN ; NaN ];
Tc_68  = [ NaN ; NaN ; NaN ];
omc_error_68 = [ NaN ; NaN ; NaN ];
Tc_error_68  = [ NaN ; NaN ; NaN ];

%-- Image #69:
omc_69 = [ NaN ; NaN ; NaN ];
Tc_69  = [ NaN ; NaN ; NaN ];
omc_error_69 = [ NaN ; NaN ; NaN ];
Tc_error_69  = [ NaN ; NaN ; NaN ];

%-- Image #70:
omc_70 = [ NaN ; NaN ; NaN ];
Tc_70  = [ NaN ; NaN ; NaN ];
omc_error_70 = [ NaN ; NaN ; NaN ];
Tc_error_70  = [ NaN ; NaN ; NaN ];

%-- Image #71:
omc_71 = [ 1.174859e+00 ; 2.388576e+00 ; -8.529941e-01 ];
Tc_71  = [ -7.797969e+01 ; -1.577671e+02 ; 4.878858e+02 ];
omc_error_71 = [ 2.633945e-03 ; 5.823505e-03 ; 7.250035e-03 ];
Tc_error_71  = [ 2.521545e+00 ; 2.253239e+00 ; 1.870698e+00 ];

%-- Image #72:
omc_72 = [ NaN ; NaN ; NaN ];
Tc_72  = [ NaN ; NaN ; NaN ];
omc_error_72 = [ NaN ; NaN ; NaN ];
Tc_error_72  = [ NaN ; NaN ; NaN ];

%-- Image #73:
omc_73 = [ NaN ; NaN ; NaN ];
Tc_73  = [ NaN ; NaN ; NaN ];
omc_error_73 = [ NaN ; NaN ; NaN ];
Tc_error_73  = [ NaN ; NaN ; NaN ];

%-- Image #74:
omc_74 = [ NaN ; NaN ; NaN ];
Tc_74  = [ NaN ; NaN ; NaN ];
omc_error_74 = [ NaN ; NaN ; NaN ];
Tc_error_74  = [ NaN ; NaN ; NaN ];

%-- Image #75:
omc_75 = [ 9.204156e-01 ; 2.531799e+00 ; -9.725509e-01 ];
Tc_75  = [ -6.707524e+01 ; -1.601185e+02 ; 5.217231e+02 ];
omc_error_75 = [ 2.625549e-03 ; 6.160519e-03 ; 7.241028e-03 ];
Tc_error_75  = [ 2.698285e+00 ; 2.411812e+00 ; 1.955715e+00 ];

%-- Image #76:
omc_76 = [ NaN ; NaN ; NaN ];
Tc_76  = [ NaN ; NaN ; NaN ];
omc_error_76 = [ NaN ; NaN ; NaN ];
Tc_error_76  = [ NaN ; NaN ; NaN ];

%-- Image #77:
omc_77 = [ NaN ; NaN ; NaN ];
Tc_77  = [ NaN ; NaN ; NaN ];
omc_error_77 = [ NaN ; NaN ; NaN ];
Tc_error_77  = [ NaN ; NaN ; NaN ];

%-- Image #78:
omc_78 = [ NaN ; NaN ; NaN ];
Tc_78  = [ NaN ; NaN ; NaN ];
omc_error_78 = [ NaN ; NaN ; NaN ];
Tc_error_78  = [ NaN ; NaN ; NaN ];

%-- Image #79:
omc_79 = [ NaN ; NaN ; NaN ];
Tc_79  = [ NaN ; NaN ; NaN ];
omc_error_79 = [ NaN ; NaN ; NaN ];
Tc_error_79  = [ NaN ; NaN ; NaN ];

%-- Image #80:
omc_80 = [ NaN ; NaN ; NaN ];
Tc_80  = [ NaN ; NaN ; NaN ];
omc_error_80 = [ NaN ; NaN ; NaN ];
Tc_error_80  = [ NaN ; NaN ; NaN ];

%-- Image #81:
omc_81 = [ NaN ; NaN ; NaN ];
Tc_81  = [ NaN ; NaN ; NaN ];
omc_error_81 = [ NaN ; NaN ; NaN ];
Tc_error_81  = [ NaN ; NaN ; NaN ];

%-- Image #82:
omc_82 = [ 3.136917e-01 ; 2.817757e+00 ; -1.182824e+00 ];
Tc_82  = [ 5.919175e+01 ; -1.332323e+02 ; 5.339936e+02 ];
omc_error_82 = [ 3.296896e-03 ; 6.208037e-03 ; 7.499121e-03 ];
Tc_error_82  = [ 2.743987e+00 ; 2.453265e+00 ; 1.979567e+00 ];

%-- Image #83:
omc_83 = [ NaN ; NaN ; NaN ];
Tc_83  = [ NaN ; NaN ; NaN ];
omc_error_83 = [ NaN ; NaN ; NaN ];
Tc_error_83  = [ NaN ; NaN ; NaN ];

%-- Image #84:
omc_84 = [ NaN ; NaN ; NaN ];
Tc_84  = [ NaN ; NaN ; NaN ];
omc_error_84 = [ NaN ; NaN ; NaN ];
Tc_error_84  = [ NaN ; NaN ; NaN ];

%-- Image #85:
omc_85 = [ NaN ; NaN ; NaN ];
Tc_85  = [ NaN ; NaN ; NaN ];
omc_error_85 = [ NaN ; NaN ; NaN ];
Tc_error_85  = [ NaN ; NaN ; NaN ];

%-- Image #86:
omc_86 = [ 1.484047e+00 ; 2.009533e+00 ; -8.842768e-01 ];
Tc_86  = [ -1.192188e+02 ; -1.424569e+02 ; 4.895409e+02 ];
omc_error_86 = [ 3.188991e-03 ; 5.542650e-03 ; 6.863931e-03 ];
Tc_error_86  = [ 2.542215e+00 ; 2.278069e+00 ; 1.834215e+00 ];

%-- Image #87:
omc_87 = [ NaN ; NaN ; NaN ];
Tc_87  = [ NaN ; NaN ; NaN ];
omc_error_87 = [ NaN ; NaN ; NaN ];
Tc_error_87  = [ NaN ; NaN ; NaN ];

%-- Image #88:
omc_88 = [ NaN ; NaN ; NaN ];
Tc_88  = [ NaN ; NaN ; NaN ];
omc_error_88 = [ NaN ; NaN ; NaN ];
Tc_error_88  = [ NaN ; NaN ; NaN ];

%-- Image #89:
omc_89 = [ NaN ; NaN ; NaN ];
Tc_89  = [ NaN ; NaN ; NaN ];
omc_error_89 = [ NaN ; NaN ; NaN ];
Tc_error_89  = [ NaN ; NaN ; NaN ];

%-- Image #90:
omc_90 = [ 1.911388e+00 ; 1.317274e+00 ; -5.779582e-01 ];
Tc_90  = [ -2.085286e+02 ; -3.575888e+01 ; 4.006486e+02 ];
omc_error_90 = [ 3.870024e-03 ; 4.565773e-03 ; 6.313594e-03 ];
Tc_error_90  = [ 2.048174e+00 ; 1.906235e+00 ; 1.793079e+00 ];

%-- Image #91:
omc_91 = [ NaN ; NaN ; NaN ];
Tc_91  = [ NaN ; NaN ; NaN ];
omc_error_91 = [ NaN ; NaN ; NaN ];
Tc_error_91  = [ NaN ; NaN ; NaN ];

%-- Image #92:
omc_92 = [ NaN ; NaN ; NaN ];
Tc_92  = [ NaN ; NaN ; NaN ];
omc_error_92 = [ NaN ; NaN ; NaN ];
Tc_error_92  = [ NaN ; NaN ; NaN ];

%-- Image #93:
omc_93 = [ NaN ; NaN ; NaN ];
Tc_93  = [ NaN ; NaN ; NaN ];
omc_error_93 = [ NaN ; NaN ; NaN ];
Tc_error_93  = [ NaN ; NaN ; NaN ];

%-- Image #94:
omc_94 = [ 2.096544e+00 ; 9.344922e-01 ; -3.279809e-01 ];
Tc_94  = [ -1.909208e+02 ; -2.156659e+01 ; 3.588030e+02 ];
omc_error_94 = [ 4.524255e-03 ; 3.962044e-03 ; 6.320145e-03 ];
Tc_error_94  = [ 1.844439e+00 ; 1.698431e+00 ; 1.709196e+00 ];

%-- Image #95:
omc_95 = [ NaN ; NaN ; NaN ];
Tc_95  = [ NaN ; NaN ; NaN ];
omc_error_95 = [ NaN ; NaN ; NaN ];
Tc_error_95  = [ NaN ; NaN ; NaN ];

%-- Image #96:
omc_96 = [ 2.261563e+00 ; 6.188053e-01 ; -2.123471e-01 ];
Tc_96  = [ -1.556864e+02 ; -1.688733e+00 ; 3.275251e+02 ];
omc_error_96 = [ 4.840756e-03 ; 3.440814e-03 ; 6.646561e-03 ];
Tc_error_96  = [ 1.683195e+00 ; 1.536346e+00 ; 1.520917e+00 ];

%-- Image #97:
omc_97 = [ NaN ; NaN ; NaN ];
Tc_97  = [ NaN ; NaN ; NaN ];
omc_error_97 = [ NaN ; NaN ; NaN ];
Tc_error_97  = [ NaN ; NaN ; NaN ];

%-- Image #98:
omc_98 = [ 2.322280e+00 ; 4.023335e-01 ; -8.394945e-02 ];
Tc_98  = [ -1.468017e+02 ; 3.106553e+01 ; 2.758023e+02 ];
omc_error_98 = [ 4.826544e-03 ; 3.030804e-03 ; 6.675075e-03 ];
Tc_error_98  = [ 1.427747e+00 ; 1.315622e+00 ; 1.333156e+00 ];

%-- Image #99:
omc_99 = [ NaN ; NaN ; NaN ];
Tc_99  = [ NaN ; NaN ; NaN ];
omc_error_99 = [ NaN ; NaN ; NaN ];
Tc_error_99  = [ NaN ; NaN ; NaN ];

%-- Image #100:
omc_100 = [ 2.110469e+00 ; -9.532027e-01 ; 4.709046e-01 ];
Tc_100  = [ -5.858862e+00 ; 8.335377e+01 ; 2.462583e+02 ];
omc_error_100 = [ 4.645413e-03 ; 3.763392e-03 ; 6.405487e-03 ];
Tc_error_100  = [ 1.286067e+00 ; 1.193896e+00 ; 1.235979e+00 ];

%-- Image #101:
omc_101 = [ NaN ; NaN ; NaN ];
Tc_101  = [ NaN ; NaN ; NaN ];
omc_error_101 = [ NaN ; NaN ; NaN ];
Tc_error_101  = [ NaN ; NaN ; NaN ];

%-- Image #102:
omc_102 = [ NaN ; NaN ; NaN ];
Tc_102  = [ NaN ; NaN ; NaN ];
omc_error_102 = [ NaN ; NaN ; NaN ];
Tc_error_102  = [ NaN ; NaN ; NaN ];

%-- Image #103:
omc_103 = [ 1.779406e+00 ; -1.625431e+00 ; 7.425656e-01 ];
Tc_103  = [ 1.115230e+02 ; 5.228237e+01 ; 2.787121e+02 ];
omc_error_103 = [ 3.757851e-03 ; 4.779954e-03 ; 6.677048e-03 ];
Tc_error_103  = [ 1.440629e+00 ; 1.324680e+00 ; 1.383684e+00 ];

%-- Image #104:
omc_104 = [ NaN ; NaN ; NaN ];
Tc_104  = [ NaN ; NaN ; NaN ];
omc_error_104 = [ NaN ; NaN ; NaN ];
Tc_error_104  = [ NaN ; NaN ; NaN ];

%-- Image #105:
omc_105 = [ NaN ; NaN ; NaN ];
Tc_105  = [ NaN ; NaN ; NaN ];
omc_error_105 = [ NaN ; NaN ; NaN ];
Tc_error_105  = [ NaN ; NaN ; NaN ];

%-- Image #106:
omc_106 = [ NaN ; NaN ; NaN ];
Tc_106  = [ NaN ; NaN ; NaN ];
omc_error_106 = [ NaN ; NaN ; NaN ];
Tc_error_106  = [ NaN ; NaN ; NaN ];

%-- Image #107:
omc_107 = [ NaN ; NaN ; NaN ];
Tc_107  = [ NaN ; NaN ; NaN ];
omc_error_107 = [ NaN ; NaN ; NaN ];
Tc_error_107  = [ NaN ; NaN ; NaN ];

%-- Image #108:
omc_108 = [ NaN ; NaN ; NaN ];
Tc_108  = [ NaN ; NaN ; NaN ];
omc_error_108 = [ NaN ; NaN ; NaN ];
Tc_error_108  = [ NaN ; NaN ; NaN ];

%-- Image #109:
omc_109 = [ NaN ; NaN ; NaN ];
Tc_109  = [ NaN ; NaN ; NaN ];
omc_error_109 = [ NaN ; NaN ; NaN ];
Tc_error_109  = [ NaN ; NaN ; NaN ];

%-- Image #110:
omc_110 = [ 5.397086e-02 ; -2.801562e+00 ; 1.361412e+00 ];
Tc_110  = [ 1.175189e+02 ; -1.153912e+02 ; 4.274682e+02 ];
omc_error_110 = [ 3.800682e-03 ; 5.551502e-03 ; 7.119475e-03 ];
Tc_error_110  = [ 2.207562e+00 ; 1.983031e+00 ; 1.597316e+00 ];

%-- Image #111:
omc_111 = [ NaN ; NaN ; NaN ];
Tc_111  = [ NaN ; NaN ; NaN ];
omc_error_111 = [ NaN ; NaN ; NaN ];
Tc_error_111  = [ NaN ; NaN ; NaN ];

%-- Image #112:
omc_112 = [ NaN ; NaN ; NaN ];
Tc_112  = [ NaN ; NaN ; NaN ];
omc_error_112 = [ NaN ; NaN ; NaN ];
Tc_error_112  = [ NaN ; NaN ; NaN ];

%-- Image #113:
omc_113 = [ 4.150629e-01 ; 2.678731e+00 ; -1.182173e+00 ];
Tc_113  = [ 1.389932e+01 ; -1.399651e+02 ; 4.434536e+02 ];
omc_error_113 = [ 2.958491e-03 ; 5.867250e-03 ; 6.735888e-03 ];
Tc_error_113  = [ 2.294195e+00 ; 2.036933e+00 ; 1.529201e+00 ];

%-- Image #114:
omc_114 = [ 3.951565e-01 ; 2.675464e+00 ; -1.199469e+00 ];
Tc_114  = [ 6.266730e+01 ; -1.435827e+02 ; 4.936232e+02 ];
omc_error_114 = [ 3.264630e-03 ; 5.991040e-03 ; 6.900621e-03 ];
Tc_error_114  = [ 2.556296e+00 ; 2.286671e+00 ; 1.759603e+00 ];

