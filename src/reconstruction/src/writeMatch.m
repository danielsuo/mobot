% matches is a 10 x number of SIFT point matrix. The first 5 rows are image
% and world coordinates of each SIFT point from the first camera pose
% and the second 5 rows are the same from the second camera pose
%
% x1_i, y1_i, x1_w, y1_w, z1_w, x2_i, y2_i, x2_w, y2_w, z2_w
function writeMatch(match)
    out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/match';
    match3DCoordinates = match.matches([3:5 8:10], :);

    fname_out = fullfile(out_dir, ['match' num2str(match.i) '_' num2str(match.j)]);
    fout = fopen(fname_out, 'wb');
    fwrite(fout, size(match3DCoordinates, 2), 'uint32');
    fwrite(fout, match3DCoordinates, 'double');
    fclose(fout);
    
    out_dir = '/home/danielsuo/Dropbox/mobot/src/server/result/Rt';
   
    fname_out = fullfile(out_dir, ['Rt' num2str(match.i) '_' num2str(match.j)]);
    fout = fopen(fname_out, 'wb');
    fwrite(fout, match.Rt', 'double');
    fclose(fout);
end