/************************************************************************
 * MechSys - Open Library for Mechanical Systems                        *
 * Copyright (C) 2009 Sergio Torres                                     *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * any later version.                                                   *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program. If not, see <http://www.gnu.org/licenses/>  *
 ************************************************************************/

//STD
#include<iostream>
#include <list>

// MechSys
#include <mechsys/flbm/Domain.h>

enum GradCase
{
    Gx,  ///< Gradient in the x direction solely
    Gy,  ///< Gradient in the x direction solely
    Gz   ///< Gradient in the x direction solely
};

struct UserData
{
    double        rhoxmin;
    double        rhoxmax;
    double        rhoymin;
    double        rhoymax;
    double        rhozmin;
    double        rhozmax;
    std::ofstream oss_ss;       ///< file for stress strain data
};

void Setup (FLBM::Domain & dom, void * UD)
{
    UserData & dat = (*static_cast<UserData *>(UD));
    #pragma omp parallel for schedule(static) num_threads(dom.Nproc)
    for (size_t ix=0;ix<dom.Ndim(0);ix++)
    for (size_t iy=0;iy<dom.Ndim(1);iy++)
    {
        if (!dom.IsSolid[0][ix][iy][0])
        {
            double * F = dom.   F[0][ix][iy][0];
            Vec3_t * V = &dom.Vel[0][ix][iy][0];
            double * R = &dom.Rho[0][ix][iy][0];
            F[1] = 1.0/3.0*(-2*F[0]-4*F[10]-4*F[12]-4*F[14]-F[2]-2*F[3]-2*F[4]-2*F[5]-2*F[6]-4*F[8]+2*dat.rhoxmin);
            F[7] = 1.0/24.0*(-2*F[0]-4*F[10]-4*F[12]-4*F[14]-4*F[2] +F[3]-5*F[4]  +F[5]-5*F[6]+20*F[8]+2*dat.rhoxmin);
            F[9] = 1.0/24.0*(-2*F[0]+20*F[10]-4*F[12]-4*F[14]-4*F[2]+F[3]-5*F[4]-5*F[5]+F[6]-4*F[8]+2*dat.rhoxmin);
            F[11]= 1.0/24.0*(-2*F[0]-4*F[10]+20*F[12]-4*F[14]-4*F[2]-5*F[3]+F[4]  +F[5]-5*F[6]-4*F[8]+2*dat.rhoxmin);
            F[13]= 1.0/24.0*(-2*F[0]-4*F[10]-4 *F[12]+20*F[14]-4*F[2]-5*F[3]+  F[4]-5*F[5]+F[6]-4*F[8]+2*dat.rhoxmin);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }

        if (!dom.IsSolid[0][ix][iy][dom.Ndim(2)-1])
        {
            double * F = dom.   F[0][ix][iy][dom.Ndim(2)-1];
            Vec3_t * V = &dom.Vel[0][ix][iy][dom.Ndim(2)-1];
            double * R = &dom.Rho[0][ix][iy][dom.Ndim(2)-1];
            F[2] = 1/3.0* (-2*F[0]-F[1]-2*(2*F[11]+2*F[13]+F[3]+F[4]+F[5]+F[6]+2*F[7]+2*F[9]-dat.rhoxmax));
            F[8] = 1/24.0*(-2*F[0] - 4*F[1] - 4*F[11] - 4*F[13] - 5*F[3] + F[4] - 5*F[5] + F[6] +20*F[7] - 4*F[9] + 2*dat.rhoxmax);
            F[10]= 1/24.0*(-2*F[0] - 4*F[1] - 4*F[11] - 4*F[13] - 5*F[3] + F[4] + F[5] - 5*F[6] - 4*F[7] + 20*F[9] + 2*dat.rhoxmax) ;
            F[12]= 1/24.0*(-2*F[0] - 4*F[1] + 20*F[11] - 4*F[13] + F[3] - 5*F[4] - 5*F[5] + F[6] -  4*F[7] - 4*F[9] + 2*dat.rhoxmax);
            F[14]= 1/24.0*(-2*F[0] - 4*F[1] - 4*F[11] + 20*F[13] + F[3] - 5*F[4] + F[5] - 5*F[6] -  4*F[7] - 4*F[9] + 2*dat.rhoxmax);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }
    }

    #pragma omp parallel for schedule(static) num_threads(dom.Nproc)
    for (size_t ix=0;ix<dom.Ndim(0);ix++)
    for (size_t iz=0;iz<dom.Ndim(2);iz++)
    {
        if (!dom.IsSolid[0][ix][0][iz])
        {
            double * F = dom.   F[0][ix][0][iz];
            Vec3_t * V = &dom.Vel[0][ix][0][iz];
            double * R = &dom.Rho[0][ix][0][iz];
            F[3]= 1/3.0*(-2*F[0]- 2*F[1]- 4*F[10]- 4*F[11]- 4*F[13]- 2*F[2]- F[4]- 2*F[5]- 2*F[6]- 4*F[8]+ 2*dat.rhoymin);
            F[7]= 1/24.0*(-2*F[0]+ F[1]- 4*F[10]- 4*F[11]- 4*F[13]- 5*F[2]- 4*F[4]+ F[5]-  5*F[6]+ 20*F[8]+2*dat.rhoymin);
            F[9]= 1/24.0*(-2*F[0]+ F[1]+ 20*F[10]- 4*F[11]- 4*F[13]- 5*F[2]- 4*F[4]- 5*F[5]+ F[6]- 4*F[8]+ 2*dat.rhoymin);
            F[12]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[10]+ 20*F[11]- 4*F[13]+ F[2]- 4*F[4]-5*F[5]+ F[6]- 4*F[8]+ 2*dat.rhoymin);
            F[14]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[10]- 4*F[11]+ 20*F[13]+ F[2]- 4*F[4]+ F[5]-5*F[6]- 4*F[8]+ 2*dat.rhoymin);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }

        if (!dom.IsSolid[0][ix][dom.Ndim(1)-1][iz])
        {
            double * F = dom.   F[0][ix][dom.Ndim(1)-1][iz];
            Vec3_t * V = &dom.Vel[0][ix][dom.Ndim(1)-1][iz];
            double * R = &dom.Rho[0][ix][dom.Ndim(1)-1][iz];
            F[4]= 1/3.0*(-2*F[0]- 2*F[1]- 4*F[12]- 4*F[14]- 2*F[2]- F[3]- 2*F[5]- 2*F[6]- 4*F[7]- 4*F[9]+ 2*dat.rhoymax);
            F[8]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[12]- 4*F[14]+ F[2]- 4*F[3]- 5*F[5]+ F[6]+  20*F[7]- 4*F[9]+2*dat.rhoymax);
            F[10]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[12]- 4*F[14]+ F[2]- 4*F[3]+ F[5]- 5*F[6]- 4*F[7]+ 20*F[9]+2*dat.rhoymax);
            F[11]= 1/24.0*(-2*F[0]+ F[1]+ 20*F[12]- 4*F[14]- 5*F[2]- 4*F[3]+ F[5]- 5*F[6]-4*F[7]- 4*F[9]+ 2*dat.rhoymax);
            F[13]= 1/24.0*(-2*F[0]+ F[1]- 4*F[12]+ 20*F[14]- 5*F[2]- 4*F[3]- 5*F[5]+ F[6]-4*F[7]- 4*F[9]+ 2*dat.rhoymax);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }
    }

    #pragma omp parallel for schedule(static) num_threads(dom.Nproc)
    for (size_t iy=0;iy<dom.Ndim(1);iy++)
    for (size_t iz=0;iz<dom.Ndim(2);iz++)
    {
        if (!dom.IsSolid[0][0][iy][iz])
        {
            double * F = dom.   F[0][0][iy][iz];
            Vec3_t * V = &dom.Vel[0][0][iy][iz];
            double * R = &dom.Rho[0][0][iy][iz];
            F[5] = 1/3.0*(-2*F[0] - 2*F[1] - 4*F[12] - 4*F[13] - 2*F[2] - 2*F[3] - 2*F[4] - F[6] -  4*F[8] - 4*F[9] + 2*dat.rhozmin);
            F[7] = 1/24.0*(-2*F[0] + F[1] - 4*F[12] - 4*F[13] - 5*F[2] + F[3] - 5*F[4] - 4*F[6] +  20*F[8] - 4*F[9] + 2*dat.rhozmin);
            F[10] = 1/24.0*(-2*F[0] - 5*F[1] - 4*F[12] - 4*F[13] + F[2] - 5*F[3] + F[4] - 4*F[6] - 4*F[8] + 20*F[9] + 2*dat.rhozmin);
            F[11] = 1/24.0*(-2*F[0] + F[1] + 20*F[12] - 4*F[13] - 5*F[2] - 5*F[3] + F[4] - 4*F[6] - 4*F[8] - 4*F[9] + 2*dat.rhozmin);
            F[14] = 1/24.0*(-2*F[0] - 5*F[1] - 4*F[12] + 20*F[13] + F[2] + F[3] - 5*F[4] - 4*F[6] - 4*F[8] - 4*F[9] + 2*dat.rhozmin);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }

        if (!dom.IsSolid[0][dom.Ndim(0)-1][iy][iz])
        {
            double * F = dom.   F[0][dom.Ndim(0)-1][iy][iz];
            Vec3_t * V = &dom.Vel[0][dom.Ndim(0)-1][iy][iz];
            double * R = &dom.Rho[0][dom.Ndim(0)-1][iy][iz];
            F[6]= 1/3.0*(-2*F[0]- 2*F[1]- 4*F[10]- 4*F[11]- 4*F[14]- 2*F[2]- 2*F[3]- 2*F[4]- F[5]- 4*F[7]+ 2*dat.rhozmax);
            F[8]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[10]- 4*F[11]- 4*F[14]+ F[2]- 5*F[3]+ F[4]- 4*F[5]+ 20*F[7]+ 2*dat.rhozmax);
            F[9]= 1/24.0*(-2*F[0]+ F[1]+ 20*F[10]- 4*F[11]- 4*F[14]- 5*F[2]+ F[3]- 5*F[4]- 4*F[5]- 4*F[7]+ 2*dat.rhozmax);
            F[12]= 1/24.0*(-2*F[0]- 5*F[1]- 4*F[10]+ 20*F[11]- 4*F[14]+ F[2]+ F[3]- 5*F[4]-4*F[5]- 4*F[7]+ 2*dat.rhozmax);
            F[13]= 1/24.0*(-2*F[0]+ F[1]- 4*F[10]- 4*F[11]+ 20*F[14]- 5*F[2]- 5*F[3]+ F[4]-4*F[5]- 4*F[7]+ 2*dat.rhozmax);
            *V = OrthoSys::O;
            *R = 0.0;
            for (size_t k=0;k<dom.Nneigh;k++)
            {
                *V += F[k]*dom.C[k];
                *R += F[k];
            }
            *V /= *R;
        }
    }

}

void Report (FLBM::Domain & dom, void * UD)
{
    UserData & dat = (*static_cast<UserData *>(UD));
    if (dom.idx_out==0)
    {
        String fs;
        fs.Printf("flux.res");
        dat.oss_ss.open(fs.CStr(),std::ios::out);
        dat.oss_ss << Util::_10_6  <<  "Time" << Util::_8s << "Fx" << Util::_8s << "Fy" << Util::_8s << "Fz" << Util::_8s << "M" << std::endl;
    }
    Vec3_t Flux = OrthoSys::O;
    double M    = 0.0;
    size_t nc   = 0;
    for (size_t ix=0;ix<dom.Ndim(0);ix++)
    for (size_t iy=0;iy<dom.Ndim(1);iy++)
    for (size_t iz=0;iz<dom.Ndim(2);iz++)
    {
        Flux += dom.Vel[0][ix][iy][iz]*dom.Rho[0][ix][iy][iz];
        M    += dom.Rho[0][ix][iy][iz];
    }
    Flux/=M;
    dat.oss_ss << dom.Time << Util::_8s << Flux(0) << Util::_8s << Flux(1) << Util::_8s << Flux(2) << std::endl;
}


int main(int argc, char **argv) try
{
    String filekey  (argv[1]);
    String filename (filekey+".inp");
    if (!Util::FileExists(filename)) throw new Fatal("File <%s> not found",filename.CStr());
    std::ifstream infile(filename.CStr());
    size_t Nproc = 1;
    if (argc==3) Nproc = atoi(argv[2]);

    String fileLBM;
    bool   Render = true;
    size_t Step = 1;
    size_t nx0;
    size_t ny0;
    size_t nz0;
    size_t nx;
    size_t ny;
    size_t nz;
    double nu     = 1.0/6.0;
    double Tf     = 1.0e3;
    double dtOut  = 1.0e1;
    bool   oct    = true;
    double Giso   = 1.0;
    double Gdev   = 3.0;
    double th     = 0.0;
    double DPx;
    double DPy;
    double DPz;

    infile >> fileLBM;    infile.ignore(200,'\n');
    infile >> Render;     infile.ignore(200,'\n');
    infile >> Step;       infile.ignore(200,'\n');
    infile >> nx0;        infile.ignore(200,'\n');
    infile >> ny0;        infile.ignore(200,'\n');
    infile >> nz0;        infile.ignore(200,'\n');
    infile >> nx;         infile.ignore(200,'\n');
    infile >> ny;         infile.ignore(200,'\n');
    infile >> nz;         infile.ignore(200,'\n');
    infile >> nu;         infile.ignore(200,'\n');
    infile >> Tf;         infile.ignore(200,'\n');
    infile >> dtOut;      infile.ignore(200,'\n');
    infile >> oct;        infile.ignore(200,'\n');
    if (oct)
    {
        infile >> Giso;     infile.ignore(200,'\n');
        infile >> Gdev;     infile.ignore(200,'\n');
        infile >> th;       infile.ignore(200,'\n');
        DPx    = Giso/3.0 + 2.0/3.0*Gdev*sin(M_PI*th/180.0-2.0*M_PI/3.0);
        DPy    = Giso/3.0 + 2.0/3.0*Gdev*sin(M_PI*th/180.0);
        DPz    = Giso/3.0 + 2.0/3.0*Gdev*sin(M_PI*th/180.0+2.0*M_PI/3.0);
    }
    else
    {
        infile >> DPx;      infile.ignore(200,'\n');
        infile >> DPy;      infile.ignore(200,'\n');
        infile >> DPz;      infile.ignore(200,'\n');
    }
    if (!Util::FileExists(fileLBM)) throw new Fatal("Binary map not found \n");

    std::ofstream parfile("param.res");
    parfile << Util::_8s << DPx/(3.0*nx)     << Util::_8s << DPy/(3.0*ny)     << Util::_8s << DPz/(3.0*nz)  << std::endl;
    //parfile << Util::_8s << Inet(0)   << Util::_8s << Inet(1)     << Util::_8s << Inet(2)  << std::endl;
    //parfile << Util::_8s << nx        << Util::_8s << ny          << Util::_8s << nz       << std::endl;
    //parfile << Util::_8s << 1.0-Dom.Lat[0].SolidFraction()        << Util::_8s << poresize << std::endl;
    //parfile << Util::_8s << mean_area << Util::_8s << mean_volume << Util::_8s << mean_sa  << std::endl;
    parfile.close();

    hid_t file_id;
    file_id = H5Fopen(fileLBM.CStr(), H5F_ACC_RDONLY, H5P_DEFAULT);
    hid_t group_id = H5Gopen(file_id, "/t0",H5P_DEFAULT);
    if (!H5LTfind_dataset(group_id,"channel0")) throw new Fatal("The matrix name does not match \n");

    hsize_t dims[3];
    H5T_class_t *class_id; 
    size_t *type_size; 

    H5LTget_dataset_info (group_id, "channel0", dims, class_id, type_size);
    std::cout << dims[2] << " " << dims[1] << " " << dims[0] << std::endl; 
    size_t ncells = dims[0]*dims[1]*dims[2];
    iVec3_t ndims = iVec3_t(dims[2],dims[1],dims[0]);
    int * Gamma = new int[ncells];
    H5LTread_dataset_int(group_id,"channel0",Gamma);

    FLBM::Domain Dom(D3Q15, nu, iVec3_t(nx,ny,nz), 1.0, 1.0);
    UserData dat;
    Dom.UserData = &dat;


    #pragma omp parallel for schedule(static) num_threads(Nproc)
    for (size_t ix=0;ix<Dom.Ndim(0);ix++)
    for (size_t iy=0;iy<Dom.Ndim(1);iy++)
    for (size_t iz=0;iz<Dom.Ndim(2);iz++)
    {
        size_t idx = FLBM::Pt2idx(iVec3_t(ix+nx0,iy+ny0,iz+nz0),ndims);
        Dom.Initialize(0,iVec3_t(ix,iy,iz),1.0,OrthoSys::O);
        if (Gamma[idx]==1) Dom.IsSolid[0][ix][iy][iz] = true;
    }
    Dom.WriteXDMF("initial");

    dat.rhoxmin = 1.0 + 0.5*DPx;
    dat.rhoxmax = 1.0 - 0.5*DPx;
    dat.rhoymin = 1.0 + 0.5*DPy;
    dat.rhoymax = 1.0 - 0.5*DPy;
    dat.rhozmin = 1.0 + 0.5*DPz;
    dat.rhozmax = 1.0 - 0.5*DPz;


    //Solving
    Dom.Solve(Tf,dtOut,Setup,Report,filekey.CStr(),Render,Nproc);
    dat.oss_ss.close();
//
    Dom.WriteXDMF("final");

    return 0;
}
MECHSYS_CATCH


