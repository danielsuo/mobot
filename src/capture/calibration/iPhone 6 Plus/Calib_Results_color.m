% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 553.377362394225884 ; 550.423721447655907 ];

%-- Principal point:
cc = [ 321.569560465750612 ; 237.528024742515754 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.082094117838074 ; -0.131669747788267 ; -0.000133766345675 ; 0.002124914172210 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 2.246776533724851 ; 2.320947128028008 ];

%-- Principal point uncertainty:
cc_error = [ 2.892322518128681 ; 2.652427200926551 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.013184125228495 ; 0.046602388568730 ; 0.001605955768597 ; 0.002205948258478 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 480;


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
omc_1 = [ 1.899646e+00 ; 1.949056e+00 ; 5.484241e-01 ];
Tc_1  = [ -8.853806e+01 ; -1.076471e+02 ; 3.055799e+02 ];
omc_error_1 = [ 4.904392e-03 ; 3.976135e-03 ; 7.415784e-03 ];
Tc_error_1  = [ 1.670918e+00 ; 1.496406e+00 ; 1.796418e+00 ];

%-- Image #2:
omc_2 = [ NaN ; NaN ; NaN ];
Tc_2  = [ NaN ; NaN ; NaN ];
omc_error_2 = [ NaN ; NaN ; NaN ];
Tc_error_2  = [ NaN ; NaN ; NaN ];

%-- Image #3:
omc_3 = [ 2.203446e+00 ; 2.228578e+00 ; -7.297529e-02 ];
Tc_3  = [ -1.206974e+02 ; -1.078325e+02 ; 3.783086e+02 ];
omc_error_3 = [ 4.643804e-03 ; 4.654703e-03 ; 1.031445e-02 ];
Tc_error_3  = [ 2.017983e+00 ; 1.837905e+00 ; 1.905883e+00 ];

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
omc_16 = [ -1.934376e+00 ; -1.925677e+00 ; 6.399799e-01 ];
Tc_16  = [ -8.942270e+01 ; -9.963030e+01 ; 4.679337e+02 ];
omc_error_16 = [ 4.582699e-03 ; 4.115128e-03 ; 8.476232e-03 ];
Tc_error_16  = [ 2.459856e+00 ; 2.212149e+00 ; 1.856429e+00 ];

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
omc_20 = [ 2.105783e+00 ; 2.078051e+00 ; -2.716911e-01 ];
Tc_20  = [ -1.121865e+02 ; -1.020128e+02 ; 4.053550e+02 ];
omc_error_20 = [ 4.364109e-03 ; 4.666651e-03 ; 9.500702e-03 ];
Tc_error_20  = [ 2.146290e+00 ; 1.930898e+00 ; 2.010172e+00 ];

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
omc_42 = [ -2.039756e+00 ; -2.094042e+00 ; -3.393263e-01 ];
Tc_42  = [ -1.220027e+02 ; -8.954993e+01 ; 3.435093e+02 ];
omc_error_42 = [ 3.849256e-03 ; 4.892314e-03 ; 9.205075e-03 ];
Tc_error_42  = [ 1.840280e+00 ; 1.718261e+00 ; 1.811200e+00 ];

%-- Image #43:
omc_43 = [ 2.148971e+00 ; 1.419628e+00 ; -2.426819e-01 ];
Tc_43  = [ -1.616444e+02 ; -8.354229e+01 ; 3.913028e+02 ];
omc_error_43 = [ 4.501900e-03 ; 4.322223e-03 ; 7.492885e-03 ];
Tc_error_43  = [ 2.081692e+00 ; 1.894865e+00 ; 1.984038e+00 ];

%-- Image #44:
omc_44 = [ 2.335969e+00 ; 1.097169e+00 ; -1.678101e-01 ];
Tc_44  = [ -1.347335e+02 ; -4.253202e+01 ; 3.722348e+02 ];
omc_error_44 = [ 4.972118e-03 ; 3.547303e-03 ; 7.684091e-03 ];
Tc_error_44  = [ 1.976366e+00 ; 1.789485e+00 ; 1.831402e+00 ];

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
omc_53 = [ 2.459038e+00 ; 4.073025e-01 ; 4.227367e-02 ];
Tc_53  = [ -8.333551e+01 ; 2.661710e+01 ; 3.723238e+02 ];
omc_error_53 = [ 5.382190e-03 ; 2.969705e-03 ; 7.857372e-03 ];
Tc_error_53  = [ 1.964474e+00 ; 1.814026e+00 ; 1.832053e+00 ];

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
omc_56 = [ 2.446673e+00 ; 1.737698e-02 ; 1.485418e-01 ];
Tc_56  = [ -7.937548e+01 ; 8.312787e+01 ; 3.492346e+02 ];
omc_error_56 = [ 5.406995e-03 ; 2.873199e-03 ; 7.655176e-03 ];
Tc_error_56  = [ 1.864703e+00 ; 1.750496e+00 ; 1.758569e+00 ];

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
omc_60 = [ 2.416500e+00 ; -3.206321e-01 ; 2.312334e-01 ];
Tc_60  = [ -4.685562e+01 ; 9.887625e+01 ; 3.461811e+02 ];
omc_error_60 = [ 5.396825e-03 ; 3.128607e-03 ; 7.555232e-03 ];
Tc_error_60  = [ 1.849408e+00 ; 1.745744e+00 ; 1.755350e+00 ];

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
omc_64 = [ 2.381263e+00 ; -7.552953e-01 ; 3.655462e-01 ];
Tc_64  = [ -1.285274e+00 ; 9.474831e+01 ; 3.210073e+02 ];
omc_error_64 = [ 5.150603e-03 ; 3.602667e-03 ; 7.482361e-03 ];
Tc_error_64  = [ 1.703684e+00 ; 1.614837e+00 ; 1.601812e+00 ];

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
omc_71 = [ 1.186082e+00 ; 2.402562e+00 ; -8.375329e-01 ];
Tc_71  = [ -4.366006e+01 ; -1.667392e+02 ; 5.059689e+02 ];
omc_error_71 = [ 2.855141e-03 ; 5.849844e-03 ; 7.644637e-03 ];
Tc_error_71  = [ 2.696705e+00 ; 2.443468e+00 ; 2.106405e+00 ];

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
omc_75 = [ 9.292011e-01 ; 2.545891e+00 ; -9.557898e-01 ];
Tc_75  = [ -3.272752e+01 ; -1.699943e+02 ; 5.390769e+02 ];
omc_error_75 = [ 2.782127e-03 ; 6.209969e-03 ; 7.662688e-03 ];
Tc_error_75  = [ 2.875312e+00 ; 2.609951e+00 ; 2.208210e+00 ];

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
omc_82 = [ 3.168610e-01 ; 2.827646e+00 ; -1.158544e+00 ];
Tc_82  = [ 9.314741e+01 ; -1.440265e+02 ; 5.518863e+02 ];
omc_error_82 = [ 3.415560e-03 ; 6.249178e-03 ; 7.917530e-03 ];
Tc_error_82  = [ 2.925357e+00 ; 2.678228e+00 ; 2.265131e+00 ];

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
omc_86 = [ 1.497327e+00 ; 2.023577e+00 ; -8.678150e-01 ];
Tc_86  = [ -8.535409e+01 ; -1.518975e+02 ; 5.089092e+02 ];
omc_error_86 = [ 3.463647e-03 ; 5.579144e-03 ; 7.226816e-03 ];
Tc_error_86  = [ 2.723052e+00 ; 2.471440e+00 ; 2.045024e+00 ];

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
omc_90 = [ 1.931415e+00 ; 1.327925e+00 ; -5.646637e-01 ];
Tc_90  = [ -1.737334e+02 ; -4.446395e+01 ; 4.216876e+02 ];
omc_error_90 = [ 4.206638e-03 ; 4.538492e-03 ; 6.655680e-03 ];
Tc_error_90  = [ 2.218644e+00 ; 2.073807e+00 ; 1.921204e+00 ];

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
omc_94 = [ 2.115549e+00 ; 9.396840e-01 ; -3.174337e-01 ];
Tc_94  = [ -1.558054e+02 ; -2.868867e+01 ; 3.788089e+02 ];
omc_error_94 = [ 4.803843e-03 ; 3.910597e-03 ; 6.622206e-03 ];
Tc_error_94  = [ 1.999460e+00 ; 1.850191e+00 ; 1.828875e+00 ];

%-- Image #95:
omc_95 = [ NaN ; NaN ; NaN ];
Tc_95  = [ NaN ; NaN ; NaN ];
omc_error_95 = [ NaN ; NaN ; NaN ];
Tc_error_95  = [ NaN ; NaN ; NaN ];

%-- Image #96:
omc_96 = [ 2.277331e+00 ; 6.197483e-01 ; -2.059837e-01 ];
Tc_96  = [ -1.205973e+02 ; -8.402325e+00 ; 3.490442e+02 ];
omc_error_96 = [ 5.095424e-03 ; 3.439493e-03 ; 6.903313e-03 ];
Tc_error_96  = [ 1.840017e+00 ; 1.693575e+00 ; 1.641070e+00 ];

%-- Image #97:
omc_97 = [ NaN ; NaN ; NaN ];
Tc_97  = [ NaN ; NaN ; NaN ];
omc_error_97 = [ NaN ; NaN ; NaN ];
Tc_error_97  = [ NaN ; NaN ; NaN ];

%-- Image #98:
omc_98 = [ 2.338775e+00 ; 4.011111e-01 ; -8.028961e-02 ];
Tc_98  = [ -1.117859e+02 ; 2.561026e+01 ; 2.981330e+02 ];
omc_error_98 = [ 5.082407e-03 ; 3.068757e-03 ; 6.920393e-03 ];
Tc_error_98  = [ 1.577450e+00 ; 1.463333e+00 ; 1.426604e+00 ];

%-- Image #99:
omc_99 = [ NaN ; NaN ; NaN ];
Tc_99  = [ NaN ; NaN ; NaN ];
omc_error_99 = [ NaN ; NaN ; NaN ];
Tc_error_99  = [ NaN ; NaN ; NaN ];

%-- Image #100:
omc_100 = [ 2.126521e+00 ; -9.648155e-01 ; 4.642308e-01 ];
Tc_100  = [ 2.872002e+01 ; 7.802587e+01 ; 2.696420e+02 ];
omc_error_100 = [ 4.736466e-03 ; 4.088676e-03 ; 6.601439e-03 ];
Tc_error_100  = [ 1.435701e+00 ; 1.359625e+00 ; 1.415310e+00 ];

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
omc_103 = [ 1.794240e+00 ; -1.639386e+00 ; 7.261900e-01 ];
Tc_103  = [ 1.459288e+02 ; 4.524378e+01 ; 3.034271e+02 ];
omc_error_103 = [ 3.746111e-03 ; 5.176313e-03 ; 6.849122e-03 ];
Tc_error_103  = [ 1.603107e+00 ; 1.525583e+00 ; 1.641038e+00 ];

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
omc_110 = [ 5.006676e-02 ; -2.817768e+00 ; 1.335116e+00 ];
Tc_110  = [ 1.516932e+02 ; -1.249898e+02 ; 4.482516e+02 ];
omc_error_110 = [ 3.970355e-03 ; 5.806342e-03 ; 7.458963e-03 ];
Tc_error_110  = [ 2.390679e+00 ; 2.215019e+00 ; 1.894598e+00 ];

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
omc_113 = [ 4.226661e-01 ; 2.690302e+00 ; -1.159426e+00 ];
Tc_113  = [ 4.810154e+01 ; -1.490955e+02 ; 4.629578e+02 ];
omc_error_113 = [ 3.120850e-03 ; 5.947766e-03 ; 7.199084e-03 ];
Tc_error_113  = [ 2.473203e+00 ; 2.246326e+00 ; 1.779808e+00 ];

%-- Image #114:
omc_114 = [ 4.012778e-01 ; 2.685854e+00 ; -1.175298e+00 ];
Tc_114  = [ 9.639894e+01 ; -1.530920e+02 ; 5.120699e+02 ];
omc_error_114 = [ 3.472080e-03 ; 6.033756e-03 ; 7.278870e-03 ];
Tc_error_114  = [ 2.736813e+00 ; 2.504750e+00 ; 2.047241e+00 ];

