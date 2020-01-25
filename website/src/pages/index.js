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
          <h2 className="Index-h2-alt">Improving Quicksort</h2>
          <h3 className="Index-h3-alt">AC's quicksort algorithm is up to 650% faster
          than other quicksort methods on random, sorted, and reversed data.</h3>
          <Link to="/docs/improving-quicksort">Start Sorting <FaArrowRight style={{ height: "12px" }}/></Link>
        </div>
        <div className="Index-cd-alt">
          user demo % ./quicksort_demo A 1000<br /><br />

          Ordered test...<br />
          system qsort: 9794.000ns<br />
          ac_sort:      4648.800ns<br /><br />

          Equal test...<br />
          system qsort: 3971.300ns<br />
          ac_sort:      3572.900ns<br /><br />

          Reverse test...<br />
          system qsort: 45616.700ns<br />
          ac_sort:      5973.600ns<br /><br />

          Slightly out of ordered test1...<br />
          system qsort: 9055.700ns<br />
          ac_sort:      39299.600ns<br /><br />

          Slightly out of ordered test2...<br />
          system qsort: 9148.600ns<br />
          ac_sort:      39708.600ns<br /><br />

          Random test...<br />
          system qsort: 93814.700ns<br />
          ac_sort:      86068.300ns
        </div>
      </div>

      <div className="Flex Index-mn">
        <div className="Index-cd-alt">
<pre>
demo/json $ make
Parse Speed (MB/sec)   ac_json  rapidjson   % gain
canada.json               1222        374  226.738%
citm_catalog.json         1145        943  21.421%
twitter.json              1000        543  84.162%
overall                   1158        511  126.614%
<br/>
Dump Speed (MB/sec)    ac_json  rapidjson   % gain
canada.json               1192        187  537.433%
citm_catalog.json          591        338  74.852%
twitter.json              1094        462  136.797%
overall                   1158        511  126.614%
</pre>
                </div>
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">JSON Parsing...Parsed.</h2>
          <h3 className="Index-h3-alt">Build lightning-fast services with AC's
          JSON parser that can parse thousands of records in milliseconds.</h3>
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
