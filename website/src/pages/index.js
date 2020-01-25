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
        Ordered test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp; &nbsp;ac_sort &nbsp; &nbsp;qsort &nbsp; &nbsp; &nbsp; % gain<br/>
         &nbsp; &nbsp; &nbsp;&nbsp;100 &nbsp; &nbsp; &nbsp; 0.059 &nbsp; &nbsp; &nbsp; 0.472 &nbsp; 700.000%<br/>
         &nbsp; &nbsp;10,000 &nbsp; &nbsp; &nbsp; 4.693 &nbsp; &nbsp; &nbsp;38.601 &nbsp; 722.523%<br/>
        1,000,000 &nbsp; &nbsp; 573.590 &nbsp; &nbsp;3929.820 &nbsp; 585.127%<br/>
        <br/>
        Equal test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp;ac_sort &nbsp; &nbsp;qsort &nbsp; &nbsp; &nbsp; % gain<br/>
         &nbsp; &nbsp; 100 &nbsp; &nbsp; &nbsp; 0.055 &nbsp; &nbsp; &nbsp; 0.287 &nbsp; 421.818%<br/>
         &nbsp; 10000 &nbsp; &nbsp; &nbsp; 4.748 &nbsp; &nbsp; &nbsp;23.796 &nbsp; 401.179%<br/>
        1000000 &nbsp; &nbsp; 561.310 &nbsp; &nbsp;2630.060 &nbsp; 368.557%<br/>
        <br/>
        Reverse test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp;ac_sort &nbsp; &nbsp;qsort &nbsp; &nbsp; &nbsp; % gain<br/>
         &nbsp; &nbsp; 100 &nbsp; &nbsp; &nbsp; 0.067 &nbsp; &nbsp; &nbsp; 2.021 &nbsp;2916.418%<br/>
         &nbsp; 10000 &nbsp; &nbsp; &nbsp; 5.432 &nbsp; &nbsp; 197.901 &nbsp;3543.244%<br/>
        1000000 &nbsp; &nbsp; 881.230 &nbsp; 20139.130 &nbsp;2185.343%<br/>
        <br/>
        Random test (time in microseconds)<br/>
        items &nbsp; &nbsp; &nbsp;ac_sort &nbsp; &nbsp;qsort &nbsp; &nbsp; &nbsp; % gain<br/>
         &nbsp; &nbsp; 100 &nbsp; &nbsp; &nbsp; 0.378 &nbsp; &nbsp; &nbsp; 1.745 &nbsp; 361.640%<br/>
         &nbsp; 10000 &nbsp; &nbsp; 419.474 &nbsp; &nbsp; 789.762 &nbsp; &nbsp;88.274%<br/>
        1000000 &nbsp; 61285.210 &nbsp;111149.170 &nbsp; &nbsp;81.364%<br/>
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
          <h2 className="Index-h2-alt">Faster JSON Parsing!</h2>
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
