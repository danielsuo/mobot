cd(fileparts(which('main')));

% im = imread('imRresult_beforeransac.jpg');
% image(im);

% pc_after = pcread('mov_afteralign.ply');
pc_ref = pcread('l.ply');
pc_mov = pcread('r.ply');

figure
subplot(2,2,1);
% pcshow(pc_after);

subplot(2,2,2);
pcshow(pc_ref);

subplot(2,2,3);
pcshow(pc_mov);

