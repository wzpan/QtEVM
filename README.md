QtEVM
=====

Yet anther C++ implementation of EVM([Eulerian Video Magnification](http://people.csail.mit.edu/mrub/vidmag/)), based on OpenCV and Qt.

Supports:

* Motion Magnification
    - spatial filter: Laplacian Pyramid
	- temporal filter: IIR bandpass filter
* Color Magnification
    - spatial filter: Gaussian Pyramid
	- temporal filter: ideal bandpass filter

More info: http://people.csail.mit.edu/mrub/vidmag/

## Dependencies ##

* Qt (>= 5.0);
* OpenCV (>= 2.0)

## Screenshot ##

![](https://raw2.github.com/wzpan/QtEVM/master/Screenshots/QtEVM.png)

## License ##

[LGPL v3](https://github.com/wzpan/QtEVM/raw/master/LICENSE)

## Credits ##

* The motion magnification module is adapted from [yusuketomoto/ofxEvm](https://github.com/yusuketomoto/ofxEvm);
* [Daniel Ron](http://web.mit.edu/dron/www/portfolio/) is a super nice person. He has experience on implementing the EVM algorithm and gave me lots of useful hints;
* Also thanks [@alessandro-gentilini](https://github.com/alessandro-gentilini) , who shared a great example of [a spatial butterworth bandpass filter](https://github.com/alessandro-gentilini/opencv_exercises-butterworth) and answered some questions on temporal filtering for me.








