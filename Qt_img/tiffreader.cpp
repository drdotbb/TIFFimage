
#include <iostream>
#include <QHash>
#include <fstream>
#include "Tiffy.cpp"

using namespace std;


static QHash<int, string> m = { {1, "BYTE"},{2, "ASCII"},{3, "SHORT"},{4, "LONG"},{5, "RATIONAL"},{6, "SBYTE"},{7, "UNDEFINED"}
,{8, "SSHORT"},{9, "SLONG"},{10, "SRATIONAL"},{11, "FLOAT"},{12, "DOUBLE"} };

static QHash<int, int> m_byte = { {1, 1},{2, 1},{3, 2},{4, 4},{5, 8},{6, 1},{7, 1}
,{8, 2},{9, 4},{10, 8},{11, 4},{12, 8} };

class pixel
{
public:
    int pixel_size;
    uint8_t* data;

public:

    static void read_RGB(Tiff& image, uint8_t* buffer, int& offsetcount, pixel* p,bool endian)
    {
        p->pixel_size = 0;
        p->data = new uint8_t[image.width * image.height*3];
        unsigned int px = 0;
        unsigned int stripbytecount=0;
        uint32_t stripoffset = 0;
        if (!endian)
        {

            for (int i = 0; i < offsetcount; i++)
            {
                stripbytecount = image.stripbytecounts[i];
                stripoffset = image.stripoffsets[i];
                p->pixel_size += stripbytecount / 3;

                for (unsigned int j = stripoffset; j < stripoffset+stripbytecount; j++)
                {
                    p->data[px] = buffer[j];
                    px++;
                }


            }
        }
        else
        {

            for (int i = 0; i < offsetcount; i++)
            {
                stripbytecount = image.stripbytecounts[i];
                stripoffset = image.stripoffsets[i];
                p->pixel_size += stripbytecount / 3;

                for (unsigned int j = stripoffset; j < stripoffset+stripbytecount; j++)
                {
                    p->data[px] = buffer[j];
                    px++;
                }



            }

        }
        //cout << p->pixel_size;
        //cout << stripoffset << stripbytecount <<endl;
    }

    void Print_RGB(pixel* p)
    {
        cout << p->pixel_size;
        for (int i = 0; i < p->pixel_size; i+=3)
        {
            cout <<"Pixel Number:"<<i<<"  "<< +p->data[i] << " " << +p->data[i+1] << " " << +p->data[i+2] << endl;
        }
    }
    void save_RGB(pixel* p,unsigned char* datta)
    {
        cout << p->pixel_size;
        for (int i = 0; i < p->pixel_size*3;i++)
        {
            datta[i] = +p->data[i];
        }
    }

};


    Tiff Read_Tiff(const char* filename)
    {
        Tiff image=getEmpty();
            ifstream f(filename, ios::in | ios::binary);



            //get length of file
            f.seekg(0, f.end);
            size_t length = f.tellg();
            f.seekg(0, f.beg);
            uint8_t* buffer = new uint8_t[length];
            f.read(reinterpret_cast<char*>(buffer), length);


            static int offsetcount = 0;


            bool lilendian = false;
            if (buffer[0] == 'I' && buffer[1] == 'I')
                lilendian = true;
            else
                lilendian = false;



            if (lilendian==false)
            {



                unsigned int result=0;
                for (int i = 4; i < 8; i++)
                {
                    result = (result << 8) | buffer[i];

                }
                cout << result << endl;


                unsigned int entry_num=0;
                for (unsigned int i = result; i < result+2; i++)
                {
                    entry_num = (entry_num << 8) | buffer[i];

                }
                cout << entry_num << endl;


                for (unsigned int j = 0; j < entry_num; j++)
                {
                    cout << "entry# :" << j << endl;
                    unsigned int fentry = result + 2+j*12;
                    unsigned int tag = 0;
                    for (unsigned int i = fentry; i < fentry + 2; i++)
                    {
                        tag = (tag << 8) | buffer[i];

                    }
                    cout << "tag:" << tag << endl;

                    unsigned int type = 0;
                    for (unsigned int i = fentry + 2; i < fentry + 4; i++)
                    {
                        type = (type << 8) | buffer[i];

                    }
                    cout << "type:" << m[type] << endl;


                    unsigned int count = 0;
                    for (unsigned int i = fentry + 4; i < fentry + 8; i++)
                    {
                        count = (count << 8) | buffer[i];

                    }
                    cout << "count:" << count << endl;

                    unsigned int value = 0;
                    unsigned int* temp = new unsigned int[count];
                    if (m_byte[type] * count <= 4)
                    {
                        for (unsigned int i = fentry + 8; i < fentry + 8 + m_byte[type] * count; i++)
                        {
                            value = (value << 8) | buffer[i];

                        }
                        cout << "Value/offset:" << value << endl << endl << endl;
                    }
                    else
                    {
                        for (unsigned int i = fentry + 8; i < fentry +12; i++)
                        {
                            value = (value << 8) | buffer[i];

                        }

                        for (unsigned int t = 0; t < count; t++)
                            temp[t] = 0;
                        for (unsigned int t = 0; t < count; t++)
                        {
                            for (unsigned int i = value; i < value + m_byte[type]; i++)
                            {
                                temp[t] = (temp[t] << 8) | buffer[i];


                            }
                            value += m_byte[type];
                        }

                        cout << "Value/offset:";
                        for (unsigned int i = 0; i < count; i++)
                            cout << temp[i]<<endl;

                        cout << endl << endl;
                    }



                    if (tag == 256)
                        image.width = value;
                    if (tag == 257)
                        image.height = value;
                    if (tag == 259)
                        image.compression = value;
                    if (tag == 262)
                        image.photometric_interpretation = value;
                    if (tag == 273)
                    {
                        image.stripoffsets = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripoffsets[0] = value;
                        else
                            image.stripoffsets = temp;
                        offsetcount = count;
                    }
                    if (tag == 278)
                        image.rowsperstrip = value;
                    if (tag == 279)
                    {
                        image.stripbytecounts = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripbytecounts[0] = value;
                        else
                            image.stripbytecounts = temp;
                    }


                }

                //cout << endl << image.stripbytecounts[0] << " " << image.stripoffsets[0]<<endl;



            }

            else
            {



                unsigned int result = 0;
                for (int i = 7; i > 3 ;i--)
                {
                    result = (result << 8) | buffer[i];

                }
                cout << result << endl;


                unsigned int entry_num = 0;
                for (unsigned int i = result + 1; i > result -1; i--)
                {
                    entry_num = (entry_num << 8) | buffer[i];

                }
                cout << entry_num << endl;


                for (unsigned int j = 0; j < entry_num; j++)
                {
                    cout << "entry# :" << j << endl;
                    unsigned int fentry = result + 2 + j * 12;
                    unsigned int tag = 0;
                    for (unsigned int i = fentry + 1; i > fentry -1; i--)
                    {
                        tag = (tag << 8) | buffer[i];

                    }
                    cout << "tag:" << tag << endl;

                    unsigned int type = 0;
                    for (unsigned int i = fentry + 3; i > fentry + 1; i--)
                    {
                        type = (type << 8) | buffer[i];

                    }
                    cout << "type:" << m[type] << endl;


                    unsigned int count = 0;
                    for (unsigned int i = fentry + 7; i > fentry + 3; i--)
                    {
                        count = (count << 8) | buffer[i];

                    }
                    cout << "count:" << count << endl;

                    unsigned int value = 0;
                    unsigned int* temp = new unsigned int[count];
                    if (m_byte[type] * count <= 4)
                    {
                        for (unsigned int i = fentry + 7 + m_byte[type] * count; i > fentry + 7; i--)
                        {
                            value = (value << 8) | buffer[i];

                        }
                        cout << "Value/offset:" << value << endl << endl << endl;
                    }
                    else
                    {
                        for (unsigned int i = fentry + 11; i > fentry + 7; i--)
                        {
                            value = (value << 8) | buffer[i];

                        }

                        for (unsigned int t = 0; t < count; t++)
                            temp[t] = 0;
                        for (unsigned int t = 0; t <count; t++)
                        {
                            for (unsigned int i = value-1 + m_byte[type]; i > value-1; i--)
                            {
                                temp[t] = (temp[t] << 8) | buffer[i];


                            }
                            value += m_byte[type];
                        }

                        cout << "Value/offset:";
                        for (unsigned int i = 0; i < count; i++)
                            cout << temp[i] << endl;

                        cout << endl << endl;
                    }



                    if (tag == 256)
                        image.width = value;
                    if (tag == 257)
                        image.height = value;
                    if (tag == 259)
                        image.compression = value;
                    if (tag == 262)
                        image.photometric_interpretation = value;
                    if (tag == 273)
                    {
                        image.stripoffsets = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripoffsets[0] = value;
                        else
                        {
                            for(unsigned int i=0;i<count;i++)
                                image.stripoffsets[i] =temp[i];
                        }
                        offsetcount = count;
                    }
                    if (tag == 278)
                        image.rowsperstrip = value;
                    if (tag == 279)
                    {
                        image.stripbytecounts = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripbytecounts[0] = value;
                        else
                        {
                            for (unsigned int i = 0; i < count; i++)
                                image.stripbytecounts[i] = temp[i];
                        }
                    }


                }

                //cout << endl << image.stripbytecounts[0] << " " << image.stripoffsets[0]<<endl;
                }


        return image;


    }

    pixel* Read_pixel(const char* filename)
    {
        Tiff image=getEmpty();
            ifstream f(filename, ios::in | ios::binary);



            //get length of file
            f.seekg(0, f.end);
            size_t length = f.tellg();
            f.seekg(0, f.beg);
            uint8_t* buffer = new uint8_t[length];
            f.read(reinterpret_cast<char*>(buffer), length);


            static int offsetcount = 0;


            bool lilendian = false;
            if (buffer[0] == 'I' && buffer[1] == 'I')
                lilendian = true;
            else
                lilendian = false;



            if (lilendian==false)
            {



                unsigned int result=0;
                for (int i = 4; i < 8; i++)
                {
                    result = (result << 8) | buffer[i];

                }
                cout << result << endl;


                unsigned int entry_num=0;
                for (unsigned int i = result; i < result+2; i++)
                {
                    entry_num = (entry_num << 8) | buffer[i];

                }
                cout << entry_num << endl;


                for (unsigned int j = 0; j < entry_num; j++)
                {
                    cout << "entry# :" << j << endl;
                    unsigned int fentry = result + 2+j*12;
                    unsigned int tag = 0;
                    for (unsigned int i = fentry; i < fentry + 2; i++)
                    {
                        tag = (tag << 8) | buffer[i];

                    }
                    cout << "tag:" << tag << endl;

                    unsigned int type = 0;
                    for (unsigned int i = fentry + 2; i < fentry + 4; i++)
                    {
                        type = (type << 8) | buffer[i];

                    }
                    cout << "type:" << m[type] << endl;


                    unsigned int count = 0;
                    for (unsigned int i = fentry + 4; i < fentry + 8; i++)
                    {
                        count = (count << 8) | buffer[i];

                    }
                    cout << "count:" << count << endl;

                    unsigned int value = 0;
                    unsigned int* temp = new unsigned int[count];
                    if (m_byte[type] * count <= 4)
                    {
                        for (unsigned int i = fentry + 8; i < fentry + 8 + m_byte[type] * count; i++)
                        {
                            value = (value << 8) | buffer[i];

                        }
                        cout << "Value/offset:" << value << endl << endl << endl;
                    }
                    else
                    {
                        for (unsigned int i = fentry + 8; i < fentry +12; i++)
                        {
                            value = (value << 8) | buffer[i];

                        }

                        for (unsigned int t = 0; t < count; t++)
                            temp[t] = 0;
                        for (unsigned int t = 0; t < count; t++)
                        {
                            for (unsigned int i = value; i < value + m_byte[type]; i++)
                            {
                                temp[t] = (temp[t] << 8) | buffer[i];


                            }
                            value += m_byte[type];
                        }

                        cout << "Value/offset:";
                        for (unsigned int i = 0; i < count; i++)
                            cout << temp[i]<<endl;

                        cout << endl << endl;
                    }



                    if (tag == 256)
                        image.width = value;
                    if (tag == 257)
                        image.height = value;
                    if (tag == 259)
                        image.compression = value;
                    if (tag == 262)
                        image.photometric_interpretation = value;
                    if (tag == 273)
                    {
                        image.stripoffsets = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripoffsets[0] = value;
                        else
                            image.stripoffsets = temp;
                        offsetcount = count;
                    }
                    if (tag == 278)
                        image.rowsperstrip = value;
                    if (tag == 279)
                    {
                        image.stripbytecounts = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripbytecounts[0] = value;
                        else
                            image.stripbytecounts = temp;
                    }


                }

                //cout << endl << image.stripbytecounts[0] << " " << image.stripoffsets[0]<<endl;
                pixel* pix = new pixel[image.width * image.height];
                pix->read_RGB(image, buffer, offsetcount, pix, lilendian);

                return pix;
            }

            else
            {



                unsigned int result = 0;
                for (int i = 7; i > 3 ;i--)
                {
                    result = (result << 8) | buffer[i];

                }
                cout << result << endl;


                unsigned int entry_num = 0;
                for (unsigned int i = result + 1; i > result -1; i--)
                {
                    entry_num = (entry_num << 8) | buffer[i];

                }
                cout << entry_num << endl;


                for (unsigned int j = 0; j < entry_num; j++)
                {
                    cout << "entry# :" << j << endl;
                    unsigned int fentry = result + 2 + j * 12;
                    unsigned int tag = 0;
                    for (unsigned int i = fentry + 1; i > fentry -1; i--)
                    {
                        tag = (tag << 8) | buffer[i];

                    }
                    cout << "tag:" << tag << endl;

                    unsigned int type = 0;
                    for (unsigned int i = fentry + 3; i > fentry + 1; i--)
                    {
                        type = (type << 8) | buffer[i];

                    }
                    cout << "type:" << m[type] << endl;


                    unsigned int count = 0;
                    for (unsigned int i = fentry + 7; i > fentry + 3; i--)
                    {
                        count = (count << 8) | buffer[i];

                    }
                    cout << "count:" << count << endl;

                    unsigned int value = 0;
                    unsigned int* temp = new unsigned int[count];
                    if (m_byte[type] * count <= 4)
                    {
                        for (unsigned int i = fentry + 7 + m_byte[type] * count; i > fentry + 7; i--)
                        {
                            value = (value << 8) | buffer[i];

                        }
                        cout << "Value/offset:" << value << endl << endl << endl;
                    }
                    else
                    {
                        for (unsigned int i = fentry + 11; i > fentry + 7; i--)
                        {
                            value = (value << 8) | buffer[i];

                        }

                        for (unsigned int t = 0; t < count; t++)
                            temp[t] = 0;
                        for (unsigned int t = 0; t <count; t++)
                        {
                            for (unsigned int i = value-1 + m_byte[type]; i > value-1; i--)
                            {
                                temp[t] = (temp[t] << 8) | buffer[i];


                            }
                            value += m_byte[type];
                        }

                        cout << "Value/offset:";
                        for (unsigned int i = 0; i < count; i++)
                            cout << temp[i] << endl;

                        cout << endl << endl;
                    }



                    if (tag == 256)
                        image.width = value;
                    if (tag == 257)
                        image.height = value;
                    if (tag == 259)
                        image.compression = value;
                    if (tag == 262)
                        image.photometric_interpretation = value;
                    if (tag == 273)
                    {
                        image.stripoffsets = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripoffsets[0] = value;
                        else
                        {
                            for(unsigned int i=0;i<count;i++)
                                image.stripoffsets[i] =temp[i];
                        }
                        offsetcount = count;
                    }
                    if (tag == 278)
                        image.rowsperstrip = value;
                    if (tag == 279)
                    {
                        image.stripbytecounts = new uint32_t[count];
                        if (m_byte[type] * count <= 4)
                            image.stripbytecounts[0] = value;
                        else
                        {
                            for (unsigned int i = 0; i < count; i++)
                                image.stripbytecounts[i] = temp[i];
                        }
                    }


                }

                //cout << endl << image.stripbytecounts[0] << " " << image.stripoffsets[0]<<endl;
                pixel* pix = new pixel[image.width * image.height];
                pix->read_RGB(image, buffer, offsetcount, pix, lilendian);

                return pix;
                }





    }












