<!-- Readme template reference: https://github.com/othneildrew/Best-README-Template/pull/73 -->

<a id="readme-top"></a>

<!-- [![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url] -->

[![project_license][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/github_username/repo_name">
    <img src="images/Sleepy_tama.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">TivaGotchi</h3>
  <p align="center">
    A Tamagotchi-clone on tm4c123gh6pm.
    <br />
    <br />
    <a href="https://github.com/NunnapasTem/TivaGotchi"><strong>Explore the docs »</strong></a>
    <br />
    <a href="https://youtu.be/J6BQPwCq-WE">View Demo</a>
    &middot;
    <a href="https://github.com/NunnapasTem/TivaGotchi/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
  
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li><a href="#Game Instruction">Game Instruction</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <!-- <li><a href="#acknowledgments">Acknowledgments</a></li> -->
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About The Project

TivaGotchi is a Tamagotchi-inspired project designed by TM4C123GH6PM microcontrollers and powered by G8RTOS. This project started as a way to bring my childhood dream of owning a <a href="https://tamagotchi.fandom.com/wiki/Tamagotchi_(1996_Pet)">Tamagotchi P1</a> to life. As a part of the project, I implemented priority scheduling, critial sections, inter-process communication and semaphores of G8RTOS to manage MCU resources. The project also integrates various peripherals, including LEDs, an LCD display, and buttons, along with custom drivers to enhance functionality. 

### Current Features
- **Buttons:** Button A, B and C are controlled via I2C communication and connected to I2C GPIO expander. Refer to the image below for how each button works in the game. 
</br>
<p align="center"><img src="https://static.wikia.nocookie.net/tamagotchi/images/5/52/Tama_diagram.png/revision/latest?cb=20191126143322"
     alt="Tamagotchi P1 Button Labels"
     style="width:150px;" />
</p>

- **LCD Display:** The LCD display is communicated with Tiva C via SPI. See <a href="https://github.com/NunnapasTem/TivaGotchi/blob/main/MultimodDrivers/src/multimod_ST7789.c">multimod_ST7789.C</a> for further information on the driver.
- **Draw Sprites:** All sprites are stored as 32x32 px C arrays in <a href="https://github.com/NunnapasTem/TivaGotchi/blob/main/Pixel.h">Pixel.h</a>. Draw_Sprite() is the middle man between SPI driver and game logics to draw sprites to a specified size and location.
- **LEDs:** The LEDs signal when your Tama needs an attention i.e. got sick, needs to use the toilet etc.  
### What's Next?
- 

### Built With

- **Languages:** ARM Assembly, C
- **Hardware:** Tiva Launchpad, I2C GPIO Expander (PCA9555PW), NRF5, PWM Driver (PCA9956b), LCD display (ST7789)
- **Tools/Libraries:** G8RTOS, TivaWare Peripheral Driver Library, Stellaris ICDI

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## Game Instruction

TODO

<p align="right">(<a href="#readme-top">back to top</a>)</p>
<!-- LICENSE -->

## License

Distributed under the project_license. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->

## Contact

_Nunnapas (Nina) Temridiwong_ - ntemridiwong@gmail.com

Project Link: [https://github.com/NunnapasTem/TivaGotchi](https://github.com/NunnapasTem/TivaGotchi)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->

<!-- ## Acknowledgments

- []()
- []()
- []() -->

<!--<p align="right">(<a href="#readme-top">back to top</a>)</p> -->

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[forks-shield]: https://img.shields.io/github/forks/github_username/repo_name.svg?style=for-the-badge
[forks-url]: https://github.com/github_username/repo_name/network/members
[stars-shield]: https://img.shields.io/github/stars/github_username/repo_name.svg?style=for-the-badge
[stars-url]: https://github.com/github_username/repo_name/stargazers
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[license-shield]: https://img.shields.io/github/license/NunnapasTem/TivaGotchi?style=for-the-badge
[license-url]: https://github.com/NunnapasTem/TivaGotchi/blob/main/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/ntemridiwong
