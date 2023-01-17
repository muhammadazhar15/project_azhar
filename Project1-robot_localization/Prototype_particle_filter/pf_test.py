import numpy as np
import cv2 as cv
import math
# import scipy.stats

img = cv.imread("tes.png")
gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
ret, img_map = cv.threshold(gray, 50, 255, cv.THRESH_BINARY_INV)

map_database = np.load('data_map.npy')
print(map_database.shape)

##trajectory = np.empty(shape=(0,3))
##start_pos = [200,200,90]
trajectory = np.load('trajectory.npy')
start_pos = trajectory[0,:]

pos = start_pos
r=20 #radius robot
N = 150 #jumlah partikel
sudut = 12 #jarak tiap scan garis
weights = np.array([1.0]*N) #bobot partikel


def generate_uniform_particles(x_range, y_range, a_range, N):
  particles = np.empty((N, 3))
  particles[:, 0] = np.random.uniform(50, x_range, size=N)
  particles[:, 1] = np.random.uniform(50, y_range, size=N)
  particles[:, 2] = np.random.uniform(0, a_range, size=N)
##  for i,angle in enumerate(particles[:,2]):
##    if angle == 360:
##      particles[i,2] = 0
  return particles

def move_model(particles, x_odo, y_odo, gyro):
  N = particles.shape[0]
  err_posx = np.full(N, np.random.uniform(-4,8))
  err_posy = np.full(N, np.random.uniform(-3,9))
  err_angle = np.full(N, np.random.uniform(-3,5))
  particles[:,0] += x_odo + err_posx
  particles[:,1] += y_odo+ err_posy
  particles[:,2] += gyro + err_angle
  return particles

def calculate_distance(img,pos,r,angl):
  line_dist = []
  line_pos = np.empty(shape=(0,2))
  for i in range(0,360,angl):
    angle = int(pos[2]+i)
    if angle >= 360:
      angle -= 360
    x = int(pos[0] + r*math.cos(angle*math.pi/180))
    y = int(pos[1] + r*math.sin(angle*math.pi/180))
    x1 = x 
    y1 = y 

    angle_ratio = math.tan(angle*math.pi/180)
    if angle <= 90:
      xadd = 1
      yadd = 1
    elif angle > 90 and angle <= 180:
      xadd = -1
      yadd = 1
    elif angle >180 and angle <= 270:
      xadd = -1
      yadd = -1
    else:
      xadd = 1
      yadd = -1

    if x>=img.shape[1] or x<=0 or y>=img.shape[0] or y<=0:
      x = x1
      y = y1
      line_pos = np.vstack((line_pos,[x,y]))
      distance = math.sqrt((y-y1)**2+(x-x1)**2)
      line_dist = np.append(line_dist,distance)
      continue
    
    while img[y,x] != 255:
      if angle_ratio >= 0:
        if x==x1:
          x += xadd
        else:
          if (y-y1)/(x-x1) < angle_ratio:
            y += yadd
          else:
            x += xadd
      else:
        if x==x1:
          x += xadd
        else:
          if (y-y1)/(x-x1) < angle_ratio:
              x += xadd
          else:
              y += yadd
      if x>=img.shape[1] or x<=0 or y>=img.shape[0] or y<=0:
        x = x1
        y = y1
        break
    line_pos = np.vstack((line_pos,[x,y]))
    distance = math.sqrt((y-y1)**2+(x-x1)**2)
    line_dist = np.append(line_dist,distance)
  return line_pos,np.float32(line_dist)

def get_map_distance(data_map,pos,angl):
  data = []
  for i in range(0,360,angl):
    angle = int(pos[2]+i)
    while angle >= 360:
      angle -= 360
    data = np.append(data,data_map[int(pos[0]),int(pos[1]),angle])
  return np.float32(data)

def move_random_particles(particles):
  N = particles.shape[0]
  particles[:,0] += np.random.uniform(-20,20,size=N)
  particles[:,1] += np.random.uniform(-20,20,size=N)
  particles[:,2] += np.random.uniform(-45,45,size=N)
  return particles
  
##particles = generate_uniform_particles(img_map.shape[0],img_map.shape[1], 360, N)
particles = generate_uniform_particles(img_map.shape[0]-50,img_map.shape[1]-50, 360, N)

##while(1):
##  trajectory = np.vstack((trajectory,pos))
for traject in trajectory[1:,:]:
  prev_pos = pos
  pos = traject
  odo = pos - prev_pos

  particles = move_model(particles,odo[0],odo[1],odo[2])
  
  #draw maps
  draw_map = cv.cvtColor(img_map.copy(), cv.COLOR_GRAY2BGR)
  #draw robot pos and angle
  cv.circle(draw_map, (int(pos[0]),int(pos[1])), 5, (255,0,0), -1)
  cv.circle(draw_map, (int(pos[0]),int(pos[1])), r-2, (255,0,0), 2)
  cv.line(draw_map, (int(pos[0]),int(pos[1])),
          (int(pos[0]+r*math.cos(pos[2]*math.pi/180)),
           int(pos[1]+r*math.sin(pos[2]*math.pi/180))),(0,255,0), 4)
##  #draw particles pos and angle
##  for particle in particles:
##    cv.circle(draw_map,(int(particle[0]),int(particle[1])),10-2,(255,255,255),1)
##    cv.line(draw_map, (int(particle[0]),int(particle[1])),
##          (int(particle[0]+15*math.cos(particle[2]*math.pi/180)),
##            int(particle[1]+15*math.sin(particle[2]*math.pi/180))),(0,255,0),1)

  line_pos,line_dist = calculate_distance(img_map,pos,r,sudut)
  for line in line_pos:
    cv.circle(draw_map, (int(line[0]),int(line[1])), 5, (0,0,255), -1)

  weights.fill(1.)
  for i,particle in enumerate(particles):
##    par_pos,par_dist = calculate_distance(img_map,particle,r,30)
    par_dist = get_map_distance(map_database,particle,sudut)
    for j,particle_dist in enumerate(par_dist):
##      weights[i] *= scipy.stats.norm(particle_dist, 30).pdf(line_dist[j])
      weights[i] *= math.exp(-0.5*(((particle_dist-line_dist[j])/30)**2)) / (30*math.sqrt(2*math.pi)) 
##      exp(-(pow((mu - x), 2)) / (pow(sigma, 2)) / 2.0) / sqrt(2.0 * M_PI * (pow(sigma, 2)));
  weights += 1.e-300 # avoid round-off to zero
  weights /= sum(weights)

  N = len(weights)
##  positions = (np.arange(N) + np.random.random()) / N ##random digunakan untuk mengecek bobot pertama
  positions = (np.arange(N) + 1.0) / N
  indexes = np.zeros(N, 'i') #tipe data integer
  cumulative_sum = np.cumsum(weights)
  i, j = 0, 0
  while i < N and j < N:
      if positions[i] < cumulative_sum[j]:
          indexes[i] = j
          i += 1
      else:
          j += 1

  for index in indexes:
    cv.circle(draw_map,(int(particles[index,0]),int(particles[index,1])),10-2,(255,0,255),1)
    cv.line(draw_map, (int(particles[index,0]),int(particles[index,1])),
          (int(particles[index,0]+15*math.cos(particles[index,2]*math.pi/180)),
           int(particles[index,1]+15*math.sin(particles[index,2]*math.pi/180))),(255,255,0),1)

  particles[:] = particles[indexes]
  weights[:] = weights[indexes]
  weights /= np.sum(weights)
  print(np.max(weights))
  particles = move_random_particles(particles)
  
  cv.imshow("map",draw_map)
  data_in = cv.waitKey(1)
  if data_in & 0xFF == 27:
    break
##  if data_in & 0xFF == ord('w'):
##    pos[0] += int(15*math.cos(pos[2]*math.pi/180))
##    pos[1] += int(15*math.sin(pos[2]*math.pi/180))
##  if data_in & 0xFF == ord('s'):
##    pos[0] -= int(15*math.cos(pos[2]*math.pi/180))
##    pos[1] -= int(15*math.sin(pos[2]*math.pi/180))
##  if data_in & 0xFF == ord('d'):
##    pos[2] += 6
##  if data_in & 0xFF == ord('a'):
##    pos[2] -= 6
##    
##  if pos[0]+(r+5) > draw_map.shape[0]:
##    pos[0] = draw_map.shape[0]-(r+5)
##  elif pos[0]-(r+5) < 0:
##    pos[0] = 0+(r+5)
##  if pos[1]+(r+5) > draw_map.shape[1]:
##    pos[1] = draw_map.shape[1]-(r+5)
##  elif pos[1]-(r+5) < 0:
##    pos[1] = 0+(r+5)
##  if pos[2] >= 360:
##    pos[2] -=360
##  elif pos[2] < 0:
##    pos[2] +=360
    
##np.save('trajectory.npy', trajectory) 
cv.destroyAllWindows()

