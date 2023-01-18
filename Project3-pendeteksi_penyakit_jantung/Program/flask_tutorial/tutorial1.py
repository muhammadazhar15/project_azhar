import os
import numpy as np
from tensorflow.keras.models import load_model
import librosa
from flask import Flask, flash, render_template, request, url_for, redirect
from werkzeug.utils import secure_filename

###NN Heart Disease
data_in = np.zeros((1, 13))
model = load_model("heartdisease_model.h5")
mean = np.load("mean.npy")
std = np.load("std.npy")
kelas = ["No Heart Disease", "Heart Disease"]

###NN Heart Sound
model_sound = load_model("heartsound_model.h5")
kelas_sound = ["Artifact", "Murmur", "Normal", "Extrahls"]
def wav2mfcc(file_path, duration=5, sr=22050):
  X, sr = librosa.load(file_path, sr=sr, duration=duration,res_type='kaiser_fast') 
  dur = librosa.get_duration(y=X, sr=sr)
  y = X
  while (dur<duration):
    y = np.append(y,y)
    dur = librosa.get_duration(y=y, sr=sr)
  if (dur >= duration):
    sound = y[0:duration*sr] 
  mfcc = librosa.feature.mfcc(sound, sr=22050,n_mfcc=15)
  return mfcc

###Flask Web
app = Flask(__name__)
app.config["SECRET_KEY"] = "asdx"
app.config["UPLOAD_FOLDER"] = "./sound_upload/"

@app.route("/")
def home():
	return render_template("index.html")

@app.route("/heartdisease", methods=["POST", "GET"])
def heartdisease():
	if request.method == "POST":
		data_in[0,0] = float(request.form["age"])
		data_in[0,1] = float(request.form["sex"])
		data_in[0,2] = float(request.form["cp"])
		data_in[0,3] = float(request.form["trestbps"])
		data_in[0,4] = float(request.form["chol"])
		data_in[0,5] = float(request.form["fbs"])
		data_in[0,6] = float(request.form["restecg"])
		data_in[0,7] = float(request.form["thalach"])
		data_in[0,8] = float(request.form["exang"])
		data_in[0,9] = float(request.form["oldpeak"])
		data_in[0,10] = float(request.form["slope"])
		data_in[0,11] = float(request.form["ca"])
		data_in[0,12] = float(request.form["thal"])
		input_data = data_in - mean
		input_data /= std	
		output_data = kelas[np.argmax(model.predict(input_data))]
		# print(output_data)
		return redirect(url_for("result",res=output_data))
	else: 
		return render_template("heartdisease.html")

@app.route("/heartsound", methods=["GET", "POST"])
def heartsound():
	if request.method == "GET":
		return render_template("heartsound.html")
	elif request.method == "POST":
		if 'file' not in request.files:
			flash("No file part")
			return redirect(url_for('heartsound'))
		data_sound = request.files.get("file")
		if data_sound.filename == '':
			flash("No selected file")
			return redirect(url_for('heartsound'))
		sound_name = secure_filename(data_sound.filename)
		data_sound.save(os.path.join(app.config["UPLOAD_FOLDER"], sound_name))
		tes = wav2mfcc(os.path.join(app.config["UPLOAD_FOLDER"], sound_name))
		output_sound = kelas_sound[np.argmax(model_sound.predict(tes.reshape(1,tes.shape[0],tes.shape[1])))]
		return redirect(url_for("result",res=output_sound))

@app.route("/result/<res>")
def result(res):
	return render_template("result.html", result = res)

if __name__ == "__main__":
	# app.run(debug=True)
	app.run(debug=True, host="0.0.0.0", port=8080)