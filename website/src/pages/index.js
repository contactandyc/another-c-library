import React, { useState } from "react";
import { Link } from "gatsby";
import { FaArrowRight } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import Preview from "../components/preview";
import "./index.css";

const cnt = [
  {
    ttl: "Quicker Sorting",
    sub: "AC’s sort algorithm is significantly faster than the standard qsort and C++’s implementation.",
    dsc: "This is accomplished through improved detection for sorted, equal, and reversed records at little to no computational cost, and hand-unrolling heap sorts for small arrays.",
    btn: "ac_sort Docs",
    pth: "/docs/ac-sort"
  },
  {
    ttl: "Faster JSON Parsing",
    sub: "Build lightning-fast services with AC's JSON parser that can parse over 1GB of json per second per CPU.",
    dsc: "",
    btn: "ac_json Docs",
    pth: "/docs/ac-json"
  },
  {
    ttl: "Reduce Memory Errors",
    sub: "AC’s allocator aids in development as memory leaks, double frees, and incorrect frees are reported.",
    dsc: "The allocation aids can be compiled out for production.  Often programs will run at 90-99% of the optimized speed with the allocation aids enabled.",
    btn: "ac_allocator Docs",
    pth: "/docs/ac-allocator"
  },
  {
    ttl: "Garbage Collection on Your Terms",
    sub: "The pool object removes the need to track allocations and lets you decide when memory should be collected.",
    dsc: "The JSON parser performance can partially be attributed to the ac_pool object. The pool makes it easy to handle allocation!",
    btn: "ac_pool Docs",
    pth: "/docs/ac-pool"
  }
];

function Hero() {
  const [cpy, setCpy] = useState("COPY");

  const cpyTxt = () => {
    // fix this later
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

function Social() {
  return (
    <div className="Index-social Flex">
      <div className="Index-social-signup Flex">
        <form name="subscribe" method="post">
          <input type="hidden" name="subscribe" value="contact" />
          <label>Subscribe for release updates: <input className="Index-social-signup-input" type="email" name="email"/>
          </label>
          <button className="Index-social-signup-btn" type="submit">
            SIGNUP
          </button>
        </form>
      </div>
      <div className="Index-github-btns">
        <iframe title="github button" src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=fork&count=false&size=large" frameBorder="0" scrolling="0" width="158px" height="30px"></iframe>
        <iframe title="github button 2" src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=star&count=false&size=large" frameBorder="0" scrolling="0" width="160px" height="30px"></iframe>
      </div>
    </div>
  );
}

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

function IndexPage() {
  return (
    <Layout>
      <SEO title="Build complex, scalable applications" />
      <div>
        <div>
        <Hero />
        </div>
        <div className="Index-preview" style={{ background: "#fafafa", margin: "100px auto", padding: "5px"}}>
          {cnt.map((val, i) => {
            return (
              <Preview
                key={i}
                ttl={cnt[i].ttl}
                sub={cnt[i].sub}
                dsc={cnt[i].dsc}
                btn={cnt[i].btn}
                pth={cnt[i].pth}
                i={i}
                />
            );
          })}
        </div>
        <Social />
        <About />
      </div>
    </Layout>
  );
}

export default IndexPage;
