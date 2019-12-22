% This demo is in the experiment
% This version is to test the possiblility of the algorithm
% Author :Wen-Tao Sun
clc
clear all;
fprintf('Task begin and paraments initialize');

% ��һ�������� ȫ����Ҫ����
%m_surface �������
%w_parameter ģ�����  H_module ��ʾģ�� no_module ����ģ��
%num_module ģ������
M_surface=[];
W_parameter=[];
H_module=[];
num_module=1;
Weight=[];
num_Images=100;  %����ͼ������
% ����ͼ������
% 
address='E:\ѧϰ\������Ŀ\��ҵ���\11.26\����\Car1\Car1\img\*.jpg';
address2='E:\ѧϰ\������Ŀ\��ҵ���\11.26\����\Car1\Car1\img\';
images=[];
file=dir(address);
% for i=1:num_Images
    c1=imread([address2,file(1).name]);
    %imshow(c1);
    images=[images c1];
% end
%���ɲ����õ���Ƶ
myObj = VideoWriter('test.avi');
writerObj.FrameRate = 30;
open(myObj);

clear c1;


% �����˲����и��� ��ʼ��������
Z_location=[86 23];  %��һ�� Ŀ�����Ͻǵ�����
Num_sampling=11; %��������
size_image=[240 320]; %ͼ��ĳߴ�
size_particle=[]; %���Ӵ�С
SearchRange=50; %������Χ
%��ʼģ������
H_module=images(86:145,23:91); % 
size_module=size(H_module);    %ģ���С����
%imshow(H_module);
size_particle=size_module;

% ���� ��˹�ֲ�
% rng_x=random('normal',0,SearchRange,1,Num_sampling);
% rng_y=random('normal',0,SearchRange,1,Num_sampling);
% ���� ���ȷֲ�
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
            Zp_location=Z_location+[rng_x(i) rng_y(j)];  %  �������������
            Zp_location(Zp_location<1)=1;
            for k=1:2
                if(Zp_location(k)+size_particle(k)-1>size_image(k))
                    Zp_location(k)= size_image(k)-size_particle(k)+1;
                end
            end
            
            
            %%   �ý����ݶ��㷨�����ر�ʾ
            gamma=0.0001;
            m_sampling=image1(Zp_location(1):Zp_location(1)+size_particle(1)-1,Zp_location(2):Zp_location(2)+size_particle(2)-1);
            %imshow(m_sampling);
            h_module=H_module;
            %W_parameter=proximalGradient(h_module,m_sampling,gamma);   ����С������Ҫ���
            delta=sum(sum(abs(h_module-m_sampling)));
            weight=exp(-gamma*delta);
            Weight=[Weight weight];
        end
    end
    
    % ģ������㷨
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
close(myObj);%�ر���Ƶ����
% analze the weight of the particle
   
