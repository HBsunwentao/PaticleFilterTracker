% This demo is in the experiment
% This version is to test the possiblility of the algorithm
% Author :Wen-Tao Sun
clc
clear all;
fprintf('Task begin and paraments initialize');

% 第一部分声明 全局重要变量
%m_surface 物体外观
%w_parameter 模板参数  H_module 表示模板 no_module 噪声模板
%num_module 模板数量
M_surface=[];
W_parameter=[];
H_module=[];
num_module=1;
Weight=[];
num_Images=100;  %载入图像数量
% 载入图像数据
% 
address='E:\学习\大型项目\毕业设计\11.26\代码\Car1\Car1\img\*.jpg';
address2='E:\学习\大型项目\毕业设计\11.26\代码\Car1\Car1\img\';
images=[];
file=dir(address);
% for i=1:num_Images
    c1=imread([address2,file(1).name]);
    %imshow(c1);
    images=[images c1];
% end
%生成测试用的视频
myObj = VideoWriter('test.avi');
writerObj.FrameRate = 30;
open(myObj);

clear c1;


% 粒子滤波进行跟踪 初始参数设置
Z_location=[86 23];  %上一次 目标左上角的坐标
Num_sampling=11; %采样数量
size_image=[240 320]; %图像的尺寸
size_particle=[]; %粒子大小
SearchRange=50; %搜索范围
%初始模板设置
H_module=images(86:145,23:91); % 
size_module=size(H_module);    %模板大小设置
%imshow(H_module);
size_particle=size_module;

% 采样 高斯分布
% rng_x=random('normal',0,SearchRange,1,Num_sampling);
% rng_y=random('normal',0,SearchRange,1,Num_sampling);
% 采样 均匀分布
step=5;
rng_x=0:step:SearchRange;
rng_x=rng_x-SearchRange/2;
rng_y=0:step:SearchRange;
rng_y=rng_y-SearchRange/2;
Num_sampling=length(rng_x)*length(rng_y);

for kk=1:num_Images
%     image1=images(:,kk*320-320+1:kk*320);
    image1=imread([address2,file(kk).name]);
    for i=1:length(rng_x)
        for j=1:length(rng_y)
            Zp_location=Z_location+[rng_x(i) rng_y(j)];  %  存在溢出的隐患
            Zp_location(Zp_location<1)=1;
            for k=1:2
                if(Zp_location(k)+size_particle(k)-1>size_image(k))
                    Zp_location(k)= size_image(k)-size_particle(k)+1;
                end
            end
            
            
            %%   用近端梯度算法进行重表示
            gamma=0.0001;
            m_sampling=image1(Zp_location(1):Zp_location(1)+size_particle(1)-1,Zp_location(2):Zp_location(2)+size_particle(2)-1);
            %imshow(m_sampling);
            h_module=H_module;
            %W_parameter=proximalGradient(h_module,m_sampling,gamma);   存在小问题需要解决
            delta=sum(sum(abs(h_module-m_sampling)));
            weight=exp(-gamma*delta);
            Weight=[Weight weight];
        end
    end
    
    % 模板更新算法
    [x y]=find(Weight==max(Weight));
    xi=floor(y/length(rng_y))+1;
    yj=mod(y,length(rng_y));
    if(yj<1)
        yj=11;
        xi=xi-1;
    end
    Z_location=Z_location+[rng_x(xi) rng_y(yj)];
    H_module=image1(Z_location(1):Z_location(1)+size_particle(1)-1,Z_location(2):Z_location(2)+size_particle(2)-1);
    Weight=[];
    %figure(1);
    
    des=drawRect(image1,[Z_location(2),Z_location(1)],[size_particle(2),size_particle(1)],5);
    imshow(des);
    writeVideo(myObj,des);
end
close(myObj);%关闭视频对象
% analze the weight of the particle
   
