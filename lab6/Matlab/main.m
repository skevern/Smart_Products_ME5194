%% Lab 9: Computer Vision
% Here we can import the test image, and the import the data from the
% keypoints using the import data tool (import the data as a matrix).
% Here we will filter out the repeating keypoints, leaving distinct points
% that we can use to determine the projection matrix.
% Steps involved here...
%   1. Extract and filter output repeating keypoints
%   2. 
clear im_pts_x im_pts_y im_label

%% Filtering the Keypoints
image_string_bl = 'blank_test_image.jpg';
image_string_x = 'test_image_label.bmp';


% the number of pixels that x,y must be seperated by to be a unique
% keypoint. This is accounts for keypoints which are the same in the world
% coordinate but vary by some small error in the image.
dx = 3;
dy = 3;
l = 1;

for i= 1:length(corners1(:,1))
    %If this is the very first point we are considering, store it!
    if i == 1
        im_pts_x(l) = corners1(i,2);
        im_pts_y(l) = corners1(i,3);
        im_label(l) = l;
        l = l+1;
        continue
    end
    
    %Condx = 1 if point is within 3 mms of existing point in x-direction
    %Condy = 1 if point is within 3 mms of existing point in y-direction
    condx = 0;
    condy = 0;
    
    %Loop through all of the important points x list
    for j = 1:length(im_pts_x)
        %Reset condx and condy values
        condx = 0;
        condy = 0;
        
        %Note, I deleted some code here that wasn't doing anything, replaced
        %rx and ry with dx and dy 
        
        %Check proximity condition in x-direction
        if corners1(i,2)<=im_pts_x(j)+dx && corners1(i,2)>=im_pts_x(j)-dx
            condx = 1;
        end
        %Check proximity condition in y-direction
        if corners1(i,3)<=im_pts_y(j)+dy && corners1(i,3)>=im_pts_y(j)-dy
            condy = 1;
        end
        
        %If point fails both condition x and y, don't add it to our list
        if condx ==1 && condy ==1
            break
        
        %If it doesn't fail both, add it to our list of important points
        elseif j == length(im_label)
            im_pts_x(l) = corners1(i,2);
            im_pts_y(l) = corners1(i,3);
            im_label(l) = l;
            l = l+1;
            break 
        end
        j;
    end
    i;
end
%% Plotting and labeling points
padx = 10;% padding around the image
pady = 10;
I_bl = imread(image_string_bl);
I_x = imread(image_string_x);
Im_bl_pad = padarray(I_bl,[padx,pady],100,'post');
% RGB = insertText(I_bl,corners1(:,2:3),corners1(:,1),'FontSize',7,'BoxColor','green');
%Im_bl_pad = insertMarker(Im_bl_pad,[im_pts_x',im_pts_y'],'+','color','red');
%RGB = insertText(Im_bl_pad,[im_pts_x',im_pts_y'],im_label','FontSize',7,'BoxColor','green');
%figure()
%image((RGB))
% figure()
% image((I_x))
% imshow(I_temp)


%% Saving the Keypoints
fileID = fopen('Key_Points1.txt','w');

% EXAMPLE:
% List of Key-Points from an image taken from the camera of interest.
% The Camera is positioned in the World Coordinates at x = 0mm, y = 0mm, z = 222 mm.
% Each Black square is equivalent to 40mm in the World coordinates.
% The world origin is (x,y,z) =(0,0,0) and it coorsponds to keypoint-36in the calibration image.  

for i = 1:length(im_label)
    fprintf(fileID,'Label:%d\t\t x:%d\t\t y:%d\n',im_label(i) ,im_pts_x(i),im_pts_y(i));
end
% fprintf(fileID,'%6.2f %12.8f\n',A);
fclose(fileID);

%% Determining Calibration Matrix
% Here you will take three pictures at three different heights that capture
% your keypoints from three different perspectives. Then you can take each
% keypoint (X,Y,Z) and its corresponding image points
% (x1,y1),(x2,y2),(x3,y3) and construct the calibration matrix using the
% routine shown in the lab documents. Then solve for the camera parameters.

% Step 1) Gather all of the keypoint data from the three images 

% Step 2) Fill the D & R matricies 

% Step 3) Use the following formula to solve for camera parameters Q = (D^T * D)^-1 * D^T * R 


