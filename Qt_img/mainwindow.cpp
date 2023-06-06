#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tiffreader.cpp"
#include <string.h>
#include <iostream>
#include <fstream>
#include <QPixmap>
#include <QMessageBox>
using namespace std;



static uint w ,h;
static unsigned int** Gray;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{



    //QImage img(reinterpret_cast<uchar*>(data),512,512,QImage::Format_RGB888);
    //bool s=img.loadFromData( reinterpret_cast<uchar*>(data), 512*512*3,const char *format =QImage::Format_RGB888);
    ui->setupUi(this);
    QPixmap pix("../Qt_img/welcome.jpg");



    int w=ui->label_pic->width();
    int h=ui->label_pic->height();
      ui->label_pic->setPixmap(pix.scaled(w,h,Qt::AspectRatioMode()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

static  QImage image;
static QImage image_g;
static int next_count=0;
static QString filepath=0;
void MainWindow::on_Browse_clicked()
{

    QFileDialog dialog(this);
        dialog.setNameFilter(tr("Images (*.tif *.jpg)"));
        dialog.setViewMode(QFileDialog::Detail);
        QString fileName = QFileDialog::getOpenFileName(this,
           tr("Open Images"), "C:/Users/behra/OneDrive/Desktop/CMPT365/Project 1", tr("Image Files (*.tif *.jpg *.tiff *.bin)"));

        if (!fileName.isEmpty())
        {
             filepath=fileName;
             next_count=0;
             ui->label_path->setText(fileName);
             Tiff img=getEmpty();
             img=Read_Tiff(fileName.toLocal8Bit().constData());
             pixel* pix=Read_pixel(fileName.toLocal8Bit().constData());
             QImage image(img.width,img.height,QImage::Format_RGB888);
             int z=0;
             for (unsigned int i = 0; i < img.height ; i++ )
             {
                 for (unsigned int j = 0; j < img.width; j++)
                 {
                     int index = z * 3;
                    QRgb rgb = qRgb( pix->data[index], //red
                                    pix->data[index + 1], //green
                                    pix->data[index + 2]);   //blue


                    image.setPixel(j,i,rgb);
                    z++;
                 }
             }

                if (next_count==0)
                {
                    ui->label_pic->setPixmap(QPixmap::fromImage(image));



                }

            w=img.width;
            h=img.height;
           Gray=new unsigned int*[h];
               for(unsigned int i=0;i<h;i++)
            {
                    Gray[i]=new unsigned int[w];
            }
               for (unsigned int i = 0; i < h ; i++ )
               {
                    for (unsigned int j = 0; j < w; j++)
                        Gray[i][j]=123;
               }




        }


}

static unsigned int Bayer_dither[8][8]={{0,48,12,60,3,51,15,63},{32,16,44,28,35,19,47,31},{8,56,4,52,11,59,7,55},{40,24,36,20,43,27,39,23},
                                        {2,50,14,62,1,49,13,61},{34,18,46,30,33,17,45,29},{10,58,6,54,9,57,5,53},{42,26,38,22,41,25,37,21}};


float Truncate(float value)
{
      if(value< 10)
            value = 10;
      if(value > 250 )
              value = 250;
      return value;
}
void MainWindow::on_pushButton_gray_clicked()
{




    int z=0;
    QMessageBox check;
    check.setText("Please first open the File");
    if(ui->label_path->text()=="File Path")
    {
        check.exec();
        next_count=0;

    }
    next_count++;
    Tiff img=getEmpty();
    img=Read_Tiff(filepath.toLocal8Bit().constData());
    QImage image_g(img.width,img.height,QImage::Format_RGB888);
    pixel* pix_gray=Read_pixel(filepath.toLocal8Bit().constData());
    if(next_count==1)
    {

     for (unsigned int i = 0; i < img.height ; i++ )
     {
         for (unsigned int j = 0; j < img.width; j++)
         {
             int index = z * 3;

              uint gray=( pix_gray->data[index]+pix_gray->data[index + 1]+pix_gray->data[index + 2] )/3;
              //cout<<gray<<endl;

            Gray[i][j]=gray;
            QRgb rgb_gray = qRgb( gray, //red
                            gray, //green
                           gray);   //blue


            image_g.setPixel(j,i,rgb_gray);
            z++;
         }
     }

        ui->label_pic->setPixmap(QPixmap::fromImage(image_g));
        QString str="Faces_gray.jpg";
        image_g.save(str,"JPEG");

    }


    else if(next_count==2)
    {



        //remapping the grayscale values
            //cout << "Remapped picture values: "<<endl;
            for (unsigned int i = 0; i < img.height; i++)
            {
                for (unsigned int j = 0; j < img.width; j++)
                {
                   Gray[i][j] = Gray[i][j] / int(256 / (8*8+1));

                }
                //cout << endl;
            }
            //cout << endl;
            QRgb rgb_dither;
            for (unsigned int i = 0; i <img.height; i++)
                {
                    for (unsigned int j = 0; j < img.width; j++)
                    {
                        unsigned int x = i % 8;
                        unsigned int y = j % 8;

                        //cout<<Gray[j][i]<<endl;
                        if (Gray[i][j] > Bayer_dither[x][y])
                           rgb_dither = qRgb( 210,210,210);
                        else
                            rgb_dither = qRgb( 0,0,0);

                           image_g.setPixel(j,i,rgb_dither);

                    }

                }
            ui->label_pic->setPixmap(QPixmap::fromImage(image_g));
            QString str="Faces_dither.jpg";
            image_g.save(str,"JPEG");
    }

    else if(next_count==3)
    {
        float Avg_Y=0;
        for(int i=0;i<pix_gray->pixel_size*3;i+=3)
            Avg_Y+=( pix_gray->data[i]+pix_gray->data[i + 1]+pix_gray->data[i + 2])/3;
        Avg_Y=Truncate(Avg_Y/(pix_gray->pixel_size));
        cout<<Avg_Y<<endl;
        double Temp_Y=0;
        double Temp_U=0;
        double Temp_V=0;
        for(int i=0;i<pix_gray->pixel_size*3;i+=3)
        {
            Temp_Y=Truncate(( pix_gray->data[i]+pix_gray->data[i + 1]+pix_gray->data[i + 2])/3);
            Temp_U=Truncate( pix_gray->data[i]*-0.147+pix_gray->data[i + 1]*-0.289+pix_gray->data[i + 2]*0.436);
            Temp_V=Truncate( pix_gray->data[i]*0.615+pix_gray->data[i + 1]*-0.515+pix_gray->data[i + 2]*-0.1000);
            if(Temp_Y<Avg_Y)
            {

                float factor = (259 * ((-Avg_Y+Temp_Y)+ 255))*1.2 / (255 * (259-(-Avg_Y+Temp_Y)));

                 pix_gray->data[i]= Truncate(factor*1.2 * ( pix_gray->data[i]   - 128) + 128);
                pix_gray->data[i + 1]= Truncate(factor * (pix_gray->data[i + 1] - 128) + 128);
                pix_gray->data[i + 2]  = Truncate(factor * (pix_gray->data[i + 2]  - 128) + 128);

                /*
                //float t=(1-(Temp_Y/Avg_Y))*0.4;
                pix_gray->data[i]=Truncate(Temp_Y+Temp_V*1.13983);
                pix_gray->data[i+1]=Truncate(Temp_Y+Temp_U*-0.39465+Temp_V*-0.58059);
                pix_gray->data[i+2]=Truncate(Temp_Y+Temp_U*2.03211);
                */

            }
            else if(Temp_Y>Avg_Y)
            {
                float factor = (259 * ((-Temp_Y+Avg_Y) + 255)) / (255 * (259-(-Temp_Y+Avg_Y) ));

                 pix_gray->data[i]= Truncate(factor*1.2 * ( pix_gray->data[i]   - 128) + 128);
                pix_gray->data[i + 1]= Truncate(factor * (pix_gray->data[i + 1] - 128) + 128);
                pix_gray->data[i + 2]  = Truncate(factor * (pix_gray->data[i + 2]  - 128) + 128);

                /*
                pix_gray->data[i]=Truncate(Temp_Y+Temp_V*1.13983);
                pix_gray->data[i+1]=Truncate(Temp_Y+Temp_U*-0.39465+Temp_V*-0.58059);
                pix_gray->data[i+2]=Truncate(Temp_Y+Temp_U*2.03211);
                */
            }
        }
        z=0;
        for (unsigned int i = 0; i < img.height ; i++ )
        {
            for (unsigned int j = 0; j < img.width; j++)
            {
                int index = z * 3;
                QRgb rgb = qRgb( pix_gray->data[index], //red
                                pix_gray->data[index + 1], //green
                                pix_gray->data[index + 2]);
                image_g.setPixel(j,i,rgb);
                z++;
            }
        }
             ui->label_pic->setPixmap(QPixmap::fromImage(image_g));
             QString str="Faces_dynamic.jpg";
             image_g.save(str,"JPEG");
    }
    else if (next_count==4)
    {
        Tiff img=getEmpty();
        img=Read_Tiff(filepath.toLocal8Bit().constData());
        pixel* pix=Read_pixel(filepath.toLocal8Bit().constData());
        QImage image(img.width,img.height,QImage::Format_RGB888);
        int z=0;
        for (unsigned int i = 0; i < img.height ; i++ )
        {
            for (unsigned int j = 0; j < img.width; j++)
            {
                int index = z * 3;
               QRgb rgb = qRgb( pix->data[index], //red
                               pix->data[index + 1], //green
                               pix->data[index + 2]);   //blue


               image.setPixel(j,i,rgb);
               z++;
            }
        }

     ui->label_pic->setPixmap(QPixmap::fromImage(image));

    }





}


