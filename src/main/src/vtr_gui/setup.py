from setuptools import setup
import os
from glob import glob

package_name = 'vtr_gui'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    # data_files=[
    #     ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
    #     ('share/' + package_name, ['package.xml']),
    #     # Root directory files - capture all files
    #     ('share/' + package_name + '/build', glob('vtr-gui/build/*')),
    
    # # Explicitly add critical files to ensure they're included
    #     ('share/' + package_name + '/build', ['vtr_gui/vtr-gui/build/index.html']),
    
    # # Static subdirectories 
    #     ('share/' + package_name + '/build/static/css', glob('vtr_gui/vtr-gui/build/static/css/*')),
    #     (  'share/' + package_name + '/build/static/js', glob('vtr_gui/vtr-gui/build/static/js/*')),
    # ],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Yuchen Wu',
    maintainer_email='cheney.wu@mail.utoronto.ca',
    description='VTR web-based GUI.',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'setup_server = vtr_gui.setup_server:main',
            'web_server = vtr_gui.web_server:main',
            'socket_server = vtr_gui.socket_server:main',
            'socket_client = vtr_gui.socket_client:main',
        ],
    },
)
