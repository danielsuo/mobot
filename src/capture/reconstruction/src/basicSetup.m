%addpath(genpath('lib'));
%addpath(genpath('IO'));
pathRoot = [pwd '/'];
addpath(pwd);			

addpath(genpath([pathRoot 'cali']));	
addpath(genpath([pathRoot 'lib/awf']));			
addpath(genpath([pathRoot 'lib/nyu']));			
addpath(genpath([pathRoot 'lib/visualindex']));
addpath(genpath([pathRoot 'lib/estimateRigidTransform']));	
addpath(genpath([pathRoot 'lib/peter']));
addpath(genpath([pathRoot 'lib/kdtree']));			
addpath(genpath([pathRoot 'lib/sift']));
addpath(genpath([pathRoot 'lib/arcball']));
addpath(genpath([pathRoot 'lib/gistdescriptor']));

warning('off', 'images:imshow:magnificationMustBeFitForDockedFigure');

%fprintf('test\n');
dbstop if error
%clc;

run('lib/vlfeat/toolbox/vl_setup');

