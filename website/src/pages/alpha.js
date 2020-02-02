import React, { useState, useEffect } from "react";
import ReactDOM from 'react-dom';
import { Link } from "gatsby";
import { FaArrowRight } from "react-icons/fa";
import { VictoryGroup, VictoryBar, VictoryChart, VictoryAxis, VictoryTooltip } from 'victory';

import Layout from "../components/layout";
import SEO from "../components/seo";
import "./index.css";

function Graph() {
  return (
    <VictoryChart
      animate={{
        duration: 2000,
        onLoad: { duration: 1000 }
      }}
      >
      <VictoryGroup offset={30}
        colorScale={"cool"}
      >
        <VictoryBar
          labelComponent={<VictoryTooltip/>}
          data={[
            { x: "Ordered", y: 393.470, label: "ac_sort 393.470ms" },
            { x: "Equal", y: 384.580, label: "ac_sort 384.580ms" },
            { x: "Reversed", y: 963.120, label: "ac_sort 963.120ms" }
          ]}
        />
        <VictoryBar
          labelComponent={<VictoryTooltip/>}
          data={[
            { x: "Ordered", y: 3315.230, label: "std::sort 3315.230ms" },
            { x: "Equal", y: 3202.080, label: "std::sort 3202.080ms" },
            { x: "Reversed", y: 5984.830, label: "std::sort 5984.830ms" }
          ]}
        />
        <VictoryBar
          labelComponent={<VictoryTooltip/>}
          data={[
            { x: "Ordered", y: 4379.690, label: "qsort 4379.690ms" },
            { x: "Equal", y: 2784.150, label: "qsort 2784.150ms" },
            { x: "Reversed", y: 22208.090, label: "qsort 22208.090ms" }
          ]}
        />
      </VictoryGroup>
    </VictoryChart>
  );
}

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
        <Graph />



        {/*<div className="Index-cd-alt">
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
        </div>*/}
      </div>

      <div className="Flex Index-mn">
        <div className="Index-cd-alt">
        user demo % ./demo6 twitter.json 1000<br />
        json_parse: 257 MB/sec, 2.342ms<br />
        json_dump: 163 MB/sec, 2.723ms<br /><br />

        user demo % ./demo6 canada.json 1000<br />
        json_parse: 245 MB/sec, 8.755ms<br />
        json_dump: 549 MB/sec, 3.907ms<br /><br />

        user demo % ./demo6 citm_catalog.json 1000<br />
        json_parse: 219 MB/sec, 7.511ms<br />
        json_dump: 109 MB/sec, 4.357ms
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
