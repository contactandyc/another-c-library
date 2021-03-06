import React from "react";
import { Link } from "gatsby";
import { FaArrowRight } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import Preview from "../components/preview";
import "./index.css";

function IndexPage() {
  return (
    <Layout>
      <SEO title="Build complex, scalable applications" />

      {/* Homepage Hero Section */}
      <div className="Index-hr">
        <h4 className="Index-url">AnotherCLibrary.com</h4>
        <div className="Index-hr-txt">
          <h2 className="Index-h2">Another C Library</h2>
          <div className="Index-cd-wrp">
            <h3 className="Index-h3">
              <code className="Index-cd">ac</code> library for building
              scalable, complex applications.
            </h3>
            <Link to="/docs/">
              <button className="Index-btn">
                Get Started?&nbsp;<div className="Index-crsr" />
              </button>
            </Link>
          </div>
        </div>
      </div>

      {/* Previews section. Edit content in ../components/preview.js */}
      <Preview />

      {/* Content by @Andy. Commenting out due to no code example.
      <div className="Flex Index-mn">
      <div className="Index-mn-full">
      <h2 className="Index-h2-alt">A C Map/Reduce Framework</h2>
      <h3 className="Index-h3-alt">The speed of C mixed with the very useful map/reduce pattern</h3>
      <p>The AC map/reduce framework is a library and as such you maintain control of every layer of complexity.</p>
      <Link to="/docs/ac-mapreduce">
        <button className="Index-btn" style={{ fontSize: "16px", margin: "10px 10px 10px 0" }}>
          Read More
        </button>
      </Link>
      </div>
      </div>*/}

      <div style={{ background: "#F2F2F2" }}>
        <div className="Grid Index-mn">
          <div>
            <h3>Goals of this Project:</h3>
            <ol>
              <li>To provide an open source collection of algorithms necessary to build complex applications</li>
              <li>To help engineers understand algorithms and C better, so that they can create their own</li>
              <li>To help people to learn what it takes to create something new</li>
              <li>Build scalable applications using technology like Kubernetes, nginx, and docker</li>
            </ol>
          </div>
          <div>
            <h3>About this Project:</h3>
            <p className="Index-p">Another C Library rebuilds, and in some cases, improves
            upon, some of the most useful Computer Science algorithms from the ground up.
            The library is Open Source and was created in late 2019, so it is
            currently still under very active development.</p>
            <p className="Index-p">Andy and Daniel Curtis created this library to serve
            as a starting point for C developers. Whether you're diving into C for
            the first time or building complex applications, Another C Library offers
            tools for both learning and building.</p>
          </div>
        </div>
      </div>
    </Layout>
  );
}

export default IndexPage;
