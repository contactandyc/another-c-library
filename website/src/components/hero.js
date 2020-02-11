import React, { useState } from "react";
import { FaArrowRight } from "react-icons/fa";
import { Link } from "gatsby";

import "../pages/index.css";

function Hero() {
  const [cpy, setCpy] = useState("COPY");

  const cpyTxt = () => {
    setCpy("COPIED");
    var copyText = document.getElementById("myInput");
    copyText.select();
    copyText.setSelectionRange(0, 99999); // For mobile
    document.execCommand("copy");
  }

  return (
    <div className="Index-hero">
      <div className="Index-hero-top">
        <h1 className="Index-hero-h1">
          <span className="Index-ac">ac</span>Library
        </h1>
        <h2 className="Index-hero-h1">
          Build scalable, complex applications <strong>faster.</strong>
        </h2>
      </div>
      <div className="Index-hero-btm">
        <div className="Index-hero-input">
          <input
            type="text"
            value="git clone https://github.comcontactandyc/another-c-library.git"
            id="myInput"
            readOnly
            />
          <button onClick={cpyTxt}>{cpy}</button>
        </div>
        <Link className="Index-hero-btn" to="/docs/">
          Get Started <FaArrowRight style={{ height: "12px" }}/>
        </Link>
      </div>
    </div>
  );
}

export default Hero;
