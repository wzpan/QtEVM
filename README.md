QtEVM
=====

Yet anther C++ implementation of EVM([Eulerian Video Magnification](http://people.csail.mit.edu/mrub/vidmag/)), based on OpenCV and Qt.

**Currently only laplacian pyramid with IIR temproal filters is implemented.**

More info: http://people.csail.mit.edu/mrub/vidmag/

## Dependencies ##

* Qt (>= 5.0);
* OpenCV (>= 2.0)

## Screenshot ##

![](https://raw2.github.com/wzpan/QtEVM/master/Screenshots/QtEVM.png)

## Credits ##

The core functions in `EVMLyprIIRProcessor` class are mostly adopted from [yusuketomoto/ofxEvm](https://github.com/yusuketomoto/ofxEvm).
