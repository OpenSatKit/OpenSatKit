## OpenSatKit - A Core Flight System (cFS) Starter Kit

The Core Flight System (cFS) provides a portable and extendible framework augmented with user applications to implement flight software 
solutions. The framework defines a layered architecture that provides a hardware and operating system independent application runtime 
environment.

![cFS Layered Architecture](https://github.com/OpenSatKit/OpenSatKit/tree/master/docs/img/cfs-layered-arch.png)

NASA maintains the cFS and community members, including NASA, provide components and distributions. 

![cFS Product Model](https://github.com/OpenSatKit/OpenSatKit/tree/master/docs/img/cfs-product-model.png)

__OpenSatKit__ is a cFS distribution that is intended to

* Provide a cFS training environment
* Provide a cFS application development environment
* Serve as a starting point for new cFS-based projects

### Documentation

[Wiki Overview](https://github.com/OpenSatKit/OpenSatKit/wiki)

[Quick Start PDF Guide](https://github.com/OpenSatKit/OpenSatKit/tree/master/docs/OSK-Quick-Start-Guide.pdf)

### Installation (Ubuntu only)

Currently only **Ubuntu 18.04 LTS** is supported. COSMOS is not compatible with Ubuntu 20.04. See COSMOS tickets [1169](https://github.com/BallAerospace/COSMOS/issues/1169) and [1171](https://github.com/BallAerospace/COSMOS/issues/1171) for details. To install OpenSatKit, copy and paste the following line in an Ubuntu terminal window to run the installation shell script. Root privilege is required to install and run OpenSatKit.

```bash
bash <(\wget -qO- https://raw.githubusercontent.com/OpenSatKit/OpenSatKit/vendor/install.sh)
```

Note when you try to run 'ruby Launcher' for the first time you'll get en error stating you need to run 'bundle install' which you should do. The 'bundle install' in the installation script doesn't seem to work and this issue is captured in OpenSatKit ticket [12](https://github.com/OpenSatKit/OpenSatKit/issues/12).

Please be patient, the installation can take around 30 minutes depending upon your internet and computer resources.
