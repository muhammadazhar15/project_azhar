#include "iris_its/mcl.h"
#include "iris_its/util.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>


#define N_PARTICLE 600
#define TO_RAD M_PI/180.0
#define TO_DEG 180.0/M_PI

#define FIELD_WIDTH 900
#define FIELD_HEIGHT 600

std::string errorTablePath = ros::package::getPath("iris_vision") + "/errortable.bin";


/**
 * @brief Initialize the MCL and generate N random particles
 * 
 */
MCL::MCL() :
  xvar(25), yvar(25), wvar(10),
  cmps(0),
  w_fast(0.0), w_slow(0.0),
  a_fast(1.0), a_slow(0.0005),
  wcmps(0.0), wvision(1.0 - wcmps)
{

}

/**
 * @brief Set initial particles
 * @param kompas
 */
void MCL::initParticles(double kompas)
{
  std::random_device x_rd, y_rd, w_rd;
  std::uniform_real_distribution<double> x_rgen(0,900), y_rgen(0,600), w_rgen(kompas-90.0,kompas+90.0);

  for(int i=0; i<N_PARTICLE; i++)
  {
    double random_kompas = w_rgen(w_rd);
    //untuk menghasilkan range 0-360
    if(random_kompas < 0){
      random_kompas = 360.0+random_kompas;
    }
    particles.push_back(Particle(x_rgen(x_rd), y_rgen(y_rd), random_kompas, wvision/N_PARTICLE, wcmps/N_PARTICLE, 1.0/N_PARTICLE));
  }
}

/**
 * @brief Update motion for every particle
 * @return State
 */
void MCL::updateMotion(double dx, double dy, double dw)
{
  mutex.lock();
  // Randomizer berdasar variabel xvar,yvar,wvar
  static std::random_device xrd, yrd, wrd;
  static std::normal_distribution<> xgen(0.0,xvar), ygen(0.0,yvar), wgen(0.0,wvar);

  for(auto& p : particles)
  {
    //menggerakan partikel dengan noise
    x(p) += dx + xgen(xrd);
    y(p) += dy + ygen(yrd); 
    w(p) += dw + wgen(wrd);

    // Normalize w
    while (w(p)>360.) {
      w(p) -= 360.;
    }
    while (w(p)<0.) {
      w(p) += 360.;
    }

  }
  mutex.unlock();
}

/**
 * @brief Update vision scanning data of the Real Robot
 * @param data List of SensorData from the Real Robot
 */
void MCL::updateSensor(const std::vector<MCL::SensorData> &data, double kompas)
{
  mutex.lock();

  n_dataSensor = data.size();
  if(n_dataSensor<=0)
    return;
  
  // Olah data scan vision Real Robot ke setiap particle
  double weight_sum(0.0);
  for(auto& p : particles)
  {
    // double err_sum(0.0);
    double err_sum(1.0);
    
    for(auto d : data)
    {
      double c = cos(w(p)*TO_RAD);
      double s = sin(w(p)*TO_RAD);

      // Translate the sensor data to the coordinate in REAL FIELD
      double world_x = -c*x(d) + s*y(d) + x(p); //punya simulator iris
      double world_y = s*x(d) + c*y(d) + y(p);  //punya simulator iris

      double distance = field.distance(world_x,world_y);

      // hitung nilai weight
      // distance = distance*distance; //rumus lama
      // err_sum += distance; //rumus lama

      err_sum *= exp(-1 * distance * distance / (2 *85.0 * 85.0));
    }
    // Calculate weight
    // double p_weight = 1.0/(err_sum)/n_data; //rumus lama
    double p_weight = err_sum * n_dataSensor;
    vis_weight(p) = p_weight;
    weight_sum += p_weight;
  }

  ///untuk menentukan status kidnap
  p_maxWeight = 0.0;
  for(auto& p : particles){
    if(p_maxWeight < vis_weight(p)){
      p_maxWeight = vis_weight(p);
    }
  }
  if(p_maxWeight < 1.0e-15){
    status_kidnap = 1;
  }
  else if(p_maxWeight > 0.5){
    status_kidnap = 0;
  }
  // ROS_INFO("max w = %f", p_maxWeight);
  // std::cout << p_maxWeight <<std::endl;

  // w_avg adalah rata2
  // double w_avg(0.0);  //untuk menghitung bobot rata2
  for(auto& p : particles){
    vis_weight(p) = vis_weight(p) / weight_sum;
    total_weight(p) = vis_weight(p); //weight dalam persen
  }
  // w_avg = weight_sum/ N_PARTICLE;  //untuk menghitung bobot rata2

  resample(kompas);
  mutex.unlock();
}

double MCL::weightPosition(const std::vector<SensorData> &data, float x_pos, float y_pos, float theta){
  // mutex.lock();

  int n_data = data.size();
  
  double err_sum = 1.0;
  for(auto d : data)
  {
    double c = cos(theta*TO_RAD);
    double s = sin(theta*TO_RAD);
    double world_x = -c*x(d) + s*y(d) + x_pos; //punya simulator iris
    double world_y = s*x(d) + c*y(d) + y_pos;  //punya simulator iris

    double distance = field.distance(world_x,world_y);

    err_sum *= exp(-1 * distance * distance / (2 *85.0 * 85.0));
  }
  // Calculate weight
  double p_weight = err_sum * n_data;
  return p_weight;
  
  // mutex.unlock();
}

/**
 * @brief Weighted Estimation of the Robot Position State (Lanjutan dari MCL::estimation())
 * 
 * @return MCL::State 
 */
MCL::State MCL::weighted_estimation()
{
  mutex.lock();
  
  double x_mean = 0.0;
  double y_mean = 0.0;

  double w_mean_sin = 0.0;
  double w_mean_cos = 0.0;
  double w_mean = 0.0;

  for(auto p : particles)
  {
    x_mean += x(p)/N_PARTICLE;
    y_mean += y(p)/N_PARTICLE;

    double s = sin(w(p)*TO_RAD);
    double c = cos(w(p)*TO_RAD);
    w_mean_sin += s/N_PARTICLE;
    w_mean_cos += c/N_PARTICLE;
  }
  w_mean = atan2(w_mean_sin,w_mean_cos);
  w_mean = w_mean * TO_DEG;

  mutex.unlock();
  return std::make_tuple(x_mean,y_mean,w_mean);
}

/**
 * @brief Reset the particles
 * 
 */
void MCL::resetParticles(bool init, double xpos, double ypos, double wpos)
{
  std::random_device xrd, yrd, wrd;

  if(init)
  {
    std::normal_distribution<double> xrg(xpos,xvar), yrg(ypos,yvar), wrg(wpos,wvar);
    for(auto& p : particles)
    {
      x(p) = xrg(xrd);
      y(p) = yrg(yrd);
      w(p) = wrg(wrd);
    }
  }
  else
  {
    std::uniform_real_distribution<double> xrg(0,900), yrg(0,600), wrg(0,360);
    for(auto& p : particles)
    {
      x(p) = xrg(xrd);
      y(p) = yrg(yrd);
      w(p) = wrg(wrd);
    }
  }
}

/**
 * @brief Resample the particles
 * 
 * 
 */
void MCL::resample(double kompas)
{
  Particles plist;

  //untuk hitung bobot terbaik
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> rg( 0.0, 1.0/N_PARTICLE );
  double r = rg(gen);
  double c = total_weight(particles[0]);
  int idx = 0;
  //untuk random partikel jika kidnap
  std::random_device x_rd, y_rd, w_rd;
  std::uniform_real_distribution<double> x_rgen(0,900), y_rgen(0,600), w_rgen(kompas-90.0,kompas+90.0);

  for(int i=0; i<N_PARTICLE; i++)
  {
    if(status_kidnap == 1)
    {
      // std::cout << "MASUK RESET PARTICLE " << kompas << std::endl;
      double random_kompas = w_rgen(w_rd);
      //untuk menghasilkan range 0-360
      if(random_kompas < 0){
        random_kompas = 360.0+random_kompas;
      }
      plist.push_back(Particle(x_rgen(x_rd), y_rgen(y_rd), random_kompas, wvision/N_PARTICLE, wcmps/N_PARTICLE, 1.0/N_PARTICLE));
    }
    else
    {
      // u dan r merupakan variabel random untuk dibandingkan dengan c yang mnyimpan totoal weight
      double u = r + ( (double)i / N_PARTICLE );
      while (u >= c) {
        idx += 1;
        
        c += total_weight(particles[idx]);
      }
      plist.push_back(particles[idx]);
    }
  }
  particles = plist;
}

/**
 * @brief Construct a new MCL::FieldMatrix::FieldMatrix object
 * 
 *  to do : buat map sendiri
 */
MCL::FieldMatrix::FieldMatrix()
{
  // untuk map lama
  // start_x = -450 - 100;
  // end_x = 450 + 100;
  // start_y = -300 - 100;
  // end_y = 300 + 100;
  
  // x_length = end_x - start_x + 1;
  // y_length = end_y - start_y + 1;

  ///untuk map baru bojar
  start_x = -450 - 100;
  end_x = 450 + 100;
  start_y = -300 - 100;
  end_y = 300 + 100;

  x_length = end_x - start_x;
  y_length = end_y - start_y;

  distance_matrix = (double*) malloc(sizeof(double)*x_length*y_length);

  // Open file stream and read it
  std::ifstream errorTable( errorTablePath , std::ios::in | std::ios::binary );
  if(errorTable.is_open()){
    std::cout << "Error Table Found" << std::endl;
    errorTable.read((char*)distance_matrix, sizeof(double)*x_length*y_length);
    errorTable.close();
  }
  else {
    std::cout << "Error Table File Not Found" << std::endl;
  }

}

/**
 * @brief Get distance seng aku gapaham :)
 * 
 * @param x 
 * @param y 
 * @return double 
 */

double MCL::FieldMatrix::distance(double x, double y)
{
  // Mod: Menengahkan titik 0,0
  x= x-450;
  y= y-300;

  if((abs((int)x)<=end_x) && (abs((int)y)<=end_y)){
    return distance_matrix[ ( (int)(y)-start_y ) * x_length + ( (int)(x)-start_x ) ];
  }
  else
  {
    return 510.0;
  }
}


