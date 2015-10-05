function calib2intrinsics(R, T, KK_left, KK_right)
    Rt = [R T / 1000];
    
    dlmwrite('intrinsics.txt', KK_left,'delimiter','\t', 'precision', 10);
    dlmwrite('intrinsics_d2c.txt', KK_right,'delimiter','\t', 'precision', 10);
    dlmwrite('intrinsics_d2c.txt', Rt,'-append', 'delimiter','\t', 'precision', 10);
end