clear; close all;

Pworld = [ 1 -1 3 ; 3 1 1]';
Pcamera = [ 3 -1 -1 ; 1 1 -3]' + 1;

DXworld = Pworld(1,1) - Pworld(1,2);
DZworld = Pworld(3,1) - Pworld(3,2);
LXZworld = sqrt(DXworld*DXworld + DZworld * DZworld);
DXworld_norm = DXworld/LXZworld;
DZworld_norm = DZworld/LXZworld;

DXcamera = Pcamera(1,1) - Pcamera(1,2);
DZcamera = Pcamera(3,1) - Pcamera(3,2);
LXZcamera = sqrt(DXcamera*DXcamera + DZcamera * DZcamera);
DXcamera_norm = DXcamera/LXZcamera;
DZcamera_norm = DZcamera/LXZcamera;

cosAngle = DXworld_norm * DXcamera_norm + DZworld_norm * DZcamera_norm;
sinAngle = DZworld_norm * DXcamera_norm - DXworld_norm * DZcamera_norm;

SXworld = Pworld(1,1) + Pworld(1,2);
SZworld = Pworld(3,1) + Pworld(3,2);

SXcamera = Pcamera(1,1) + Pcamera(1,2);
SZcamera = Pcamera(3,1) + Pcamera(3,2);

t(1) = (SXworld - cosAngle * SXcamera + sinAngle * SZcamera)/2;
t(2) = 0;
t(3) = (SZworld - sinAngle * SXcamera - cosAngle * SZcamera)/2;


R(1,1) = cosAngle;    R(1,2) = 0;    R(1,3) = -sinAngle;
R(2,1) = 0;    R(2,2) = 0;    R(2,3) = 0;
R(3,1) = sinAngle;    R(3,2) = 0;    R(3,3) = cosAngle;

plot3(Pworld(1,:), Pworld(2,:), Pworld(3,:), '-b'); hold on;
plot3(Pworld(1,:), Pworld(2,:), Pworld(3,:), '*b'); hold on;

plot3(Pcamera(1,:), Pcamera(2,:), Pcamera(3,:), '-r'); hold on;
plot3(Pcamera(1,:), Pcamera(2,:), Pcamera(3,:), '*r'); hold on;

P2world = R * Pcamera + repmat(t',1,2);

plot3(P2world(1,:), P2world(2,:), P2world(3,:), 'ok'); hold on;

axis equal
grid on
xlabel('x');
ylabel('y');
zlabel('z');
