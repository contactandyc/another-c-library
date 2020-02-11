import React from "react";
import "../pages/index.css";

function About() {
  return (
    <div style={{ background: "#f1f1f1", padding: "70px 10px"}}>
      <div className="Flex" style={{ display: "flex", maxWidth: "1200px", margin: "auto" }}>
        <div style={{ maxWidth: "550px" }}>
          <h3>About AC Library:</h3>
          <div>
            Another C Library rebuilds, and in some cases, improves upon, some of the
            most useful Computer Science algorithms from the ground up. The library is
            Open Source and was created in late 2019, so it is currently still under very
            active development.<br /><br />

            Andy and Daniel Curtis created this library to serve as a starting point for C
            developers. Whether you’re diving into C for the first time or building
            complex applications, Another C Library offers tools for both learning and
            building.
          </div>
        </div>
        <div>
          <h3>The Project Goals:</h3>
          <ol>
            <li>
              To provide an open source collection of algorithms necessary to
              build complex applications.
            </li>
            <li>
              To help engineers understand algorithms and C better, so that they
              can create their own.
            </li>
            <li>
              To help people to learn what it takes to create something new.
            </li>
            <li>
              Build scalable applications using technology like Kubernetes, nginx,
              and docker.
            </li>
          </ol>
        </div>
      </div>
    </div>
  );
}

export default About;
