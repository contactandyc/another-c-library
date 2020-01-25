import React from "react";
import { Link } from "gatsby";
import { FaArrowRight } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import "./index.css";

function IndexPage() {
  return (
    <Layout>
      <SEO title="Build complex, scalable applications" />
      <div className="Index-hr">
        <h4 className="Index-url">AnotherCLibrary.com</h4>
        <div className="Index-hr-txt">
          <h2 className="Index-h2">Another C Library</h2>
          <div className="Index-cd-wrp">
            <h3 className="Index-h3"><code className="Index-cd">ac</code> library for building scalable, complex applications.</h3>
            <Link to="/docs/">
              <button className="Index-btn">Get Started?&nbsp;<div className="Index-crsr" /></button>
            </Link>
          </div>
        </div>
      </div>

      <div className="Flex Index-mn">
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Quicker Sorting</h2>
          <h3 className="Index-h3-alt">AC's sort algorithm is significantly faster than the standard qsort.  The sort includes an improvement to the algorithm that detects if records are sorted, equal, or reversed at little to no computational cost.</h3>
          <Link to="/docs/improving-quicksort">Read More<FaArrowRight style={{ height: "12px" }}/></Link> or
          <Link to="/docs/improving-quicksort">See Article on LinkedIn<FaArrowRight style={{ height: "12px" }}/></Link>
        </div>
        <div className="Index-cd-alt">
        demo/sort $ make<br/>
        ac_sort vs std::sort<br/>
        Ordered test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; std::sort &nbsp; &nbsp; &nbsp; % gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.050 &nbsp; &nbsp; &nbsp; 0.392 &nbsp; &nbsp; &nbsp;684.000%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;3.257 &nbsp; &nbsp; &nbsp;29.778 &nbsp; &nbsp; &nbsp;814.277%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;402.610 &nbsp; 3,130.790 &nbsp; &nbsp; &nbsp;677.624%<br/>
        <br/>
        Equal test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; std::sort &nbsp; &nbsp; &nbsp; % gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.049 &nbsp; &nbsp; &nbsp; 0.295 &nbsp; &nbsp; &nbsp;502.041%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;3.770 &nbsp; &nbsp; &nbsp;26.870 &nbsp; &nbsp; &nbsp;612.732%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;388.910 &nbsp; 3,051.270 &nbsp; &nbsp; &nbsp;684.570%<br/>
        <br/>
        Reverse test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; std::sort &nbsp; &nbsp; &nbsp; % gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.067 &nbsp; &nbsp; &nbsp; 0.473 &nbsp; &nbsp; &nbsp;605.970%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;5.495 &nbsp; &nbsp; &nbsp;57.292 &nbsp; &nbsp; &nbsp;942.621%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;823.680 &nbsp; 5,955.980 &nbsp; &nbsp; &nbsp;623.094%<br/>
        <br/>
        Random test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; std::sort &nbsp; &nbsp; &nbsp; % gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.337 &nbsp; &nbsp; &nbsp; 1.545 &nbsp; &nbsp; &nbsp;358.457%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp;409.976 &nbsp; &nbsp; 511.444 &nbsp; &nbsp; &nbsp; 24.750%<br/>
        1,000,000 &nbsp; 60,723.800 &nbsp;77,476.470 &nbsp; &nbsp; &nbsp; 27.588%<br/>
        <br/>
        ac_sort vs qsort<br/>
        Ordered test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; qsort &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;% gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.050 &nbsp; &nbsp; &nbsp; &nbsp;0.511 &nbsp; &nbsp; &nbsp;922.000%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;3.257 &nbsp; &nbsp; &nbsp; 41.735 &nbsp; &nbsp; 1181.394%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;402.610 &nbsp; &nbsp;4,530.900 &nbsp; &nbsp; 1025.382%<br/>
        <br/>
        Equal test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; qsort &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;% gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.049 &nbsp; &nbsp; &nbsp; &nbsp;0.295 &nbsp; &nbsp; &nbsp;502.041%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;3.770 &nbsp; &nbsp; &nbsp; 31.206 &nbsp; &nbsp; &nbsp;727.745%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;388.910 &nbsp; &nbsp;2,808.020 &nbsp; &nbsp; &nbsp;622.023%<br/>
        <br/>
        Reverse test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; qsort &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;% gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.067 &nbsp; &nbsp; &nbsp; &nbsp;2.274 &nbsp; &nbsp; 3294.030%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp; &nbsp;5.495 &nbsp; &nbsp; &nbsp;215.620 &nbsp; &nbsp; 3823.931%<br/>
        1,000,000 &nbsp; &nbsp; &nbsp;823.680 &nbsp; 22,169.190 &nbsp; &nbsp; 2591.481%<br/>
        <br/>
        Random test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; ac_sort &nbsp; &nbsp; qsort &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;% gain &nbsp;<br/>
         &nbsp; &nbsp; &nbsp;100 &nbsp; &nbsp; &nbsp; &nbsp;0.337 &nbsp; &nbsp; &nbsp; &nbsp;1.894 &nbsp; &nbsp; &nbsp;462.018%<br/>
         &nbsp; 10,000 &nbsp; &nbsp; &nbsp;409.976 &nbsp; &nbsp; &nbsp;779.805 &nbsp; &nbsp; &nbsp; 90.207%<br/>
        1,000,000 &nbsp; 60,723.800 &nbsp;110,392.940 &nbsp; &nbsp; &nbsp; 81.795%<br/>
                </div>
      </div>

      <div className="Flex Index-mn">
        <div className="Index-cd-alt">
demo/json $ make<br/>
Parse Speed (MB/sec) &nbsp; ac_json &nbsp;rapidjson &nbsp; % gain<br/>
canada.json &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 1222 &nbsp; &nbsp; &nbsp; &nbsp;374 &nbsp;226.738%<br/>
citm_catalog.json &nbsp; &nbsp; &nbsp; &nbsp; 1145 &nbsp; &nbsp; &nbsp; &nbsp;943 &nbsp; 21.421%<br/>
twitter.json &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;1000 &nbsp; &nbsp; &nbsp; &nbsp;543 &nbsp; 84.162%<br/>
overall &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 1158 &nbsp; &nbsp; &nbsp; &nbsp;511 &nbsp;126.614%<br/>
<br/>
Dump Speed (MB/sec) &nbsp; &nbsp;ac_json &nbsp;rapidjson &nbsp; % gain<br/>
canada.json &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 1192 &nbsp; &nbsp; &nbsp; &nbsp;187 &nbsp;537.433%<br/>
citm_catalog.json &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;591 &nbsp; &nbsp; &nbsp; &nbsp;338 &nbsp; 74.852%<br/>
twitter.json &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;1094 &nbsp; &nbsp; &nbsp; &nbsp;462 &nbsp;136.797%<br/>
overall &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 1158 &nbsp; &nbsp; &nbsp; &nbsp;511 &nbsp;126.614%<br/>
                </div>
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Faster JSON Parsing</h2>
          <h3 className="Index-h3-alt">Build lightning-fast services with AC's
          JSON parser that can parse over 1GB of json per second per CPU.</h3>
          <Link to="/docs/ac-buffer">Read More <FaArrowRight style={{ height: "12px" }}/></Link>
        </div>
      </div>

      {/*<div className="Flex Index-mn">
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Improving Quicksort</h2>
          <h3 className="Index-h3-alt">Our quicksort algorithm is up to 400% faster
          than other quicksort methods on random, sorted, and reversed data.</h3>
          <Link to="">Start Sorting <FaArrowRight style={{ height: "12px" }}/></Link>
        </div>
        <div className="Index-cd-alt">

        </div>
      </div>*/}

      <p className="Index-mn" style={{ paddingTop: 0, textAlign: "center" }}><strong>Our quicksort and parser are just two of our 14 modules. Want to
      see more?<br />Clone our library and run demos in the /demo directory or read
      more in our <Link to="/docs/">Documentation</Link> and
      <Link to="/tutorial/"> Tutorial.</Link></strong></p>

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
