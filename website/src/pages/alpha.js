import React, { useState, useEffect } from "react";
import ReactDOM from 'react-dom';
import { Link } from "gatsby";
import { FaArrowRight } from "react-icons/fa"
import { Chart } from 'react-charts'

import Footer from "../components/footer";
import "./index.css";

function MyChart() {
  const data = React.useMemo(
    () => [
      {
        label: 'Series 1',
        data: [[0, 1], [1, 2], [2, 4], [3, 2], [4, 7]]
      },
      {
        label: 'Series 2',
        data: [[0, 3], [1, 1], [2, 5], [3, 6], [4, 4]]
      }
    ],
    []
  )

  const axes = React.useMemo(
    () => [
      { primary: true, type: 'linear', position: 'bottom' },
      { type: 'linear', position: 'left' }
    ],
    []
  )

  const lineChart = (
    // A react-chart hyper-responsively and continuusly fills the available
    // space of its parent element automatically
    <div
      style={{
        width: '400px',
        height: '300px'
      }}
    >
      <Chart data={data} axes={axes} />
    </div>
  );

  return lineChart;
}

function Header() {
  return (
    <header className="Alpha-h">
      <div className="Alpha-h-i"><span className="Alpha-ac">AC</span>Library</div>
      <nav className="Alpha-h-n">
        <div>Documentation</div>
        <div>Tutorial</div>
        <div>GitHub</div>
        <div>Beta</div>
      </nav>
    </header>
  );
}

function Hero() {
  const [cpy, setCpy] = useState("COPY");

  const cpyTxt = () => {
    // fix this later to be react
    setCpy("COPIED");
    var copyText = document.getElementById("myInput");
    copyText.select();
    copyText.setSelectionRange(0, 99999); // For mobile
    document.execCommand("copy");
  }

  return (
    <div className="Alpha-hr">
      <div className="Alpha-hr-top">
        <iframe src="https://www.youtube.com/embed/MMCRLJWOqdk" frameborder="0" />
        <div>
          <div className="Alpha-h-i"><span className="Alpha-ac">AC</span>Library</div>
          <div>Build scalable, complex applications <strong>faster.</strong></div>
        </div>
      </div>
      <div className="Alpha-hr-btm">
        <input type="text" value="git clone https://github.comcontactandyc/another-c-library.git" id="myInput" />
        <button onClick={cpyTxt}>{cpy}</button>
        <div>Get Started <FaArrowRight style={{ height: "12px" }}/></div>
      </div>
    </div>
  );
}

function Preview(props) {
  if (props.side === "left") {
    return (
      <div className="Alpha-prev">
        <div className="Alpha-prev-txt">
          <div>{props.ttl}</div>
          <div>{props.tag}</div>
          <div>{props.dsc}</div>
          <button>{props.btn}</button>
        </div>
        <div className="Alpha-prev-txt">
          <MyChart />
        </div>
      </div>
    );
  } else {
    return (
      <div className="Alpha-prev">
        <div className="Alpha-prev-txt">
          <MyChart />
        </div>
        <div className="Alpha-prev-txt">
          <div>{props.ttl}</div>
          <div>{props.tag}</div>
          <div>{props.dsc}</div>
          <button>{props.btn}</button>
        </div>
      </div>
    );
  }
}

function Social() {
  return (
    <div style={{ display: "flex", maxWidth: 1200, margin: "0 auto 80px auto", alignItems: "center", justifyContent: "center" }}>
      <div className="Alpha-social-signup">
        <div>Subscribe for release updates:</div>
        <input name="email"></input>
        <div className="Alpha-social-signup-btn">SIGNUP</div>
      </div>
      <div className="Alpha-github-btns">
        <iframe src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=fork&count=true&size=large" frameborder="0" scrolling="0" width="158px" height="30px"></iframe>
        <iframe src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=star&count=true&size=large" frameborder="0" scrolling="0" width="160px" height="30px"></iframe>
      </div>
    </div>
  );
}

function About() {
  return (
    <div style={{ background: "#f1f1f1", padding: "70px"}}>
    <div style={{ display: "flex", maxWidth: "1200px", margin: "auto" }}>
      <div style={{ maxWidth: "550px", marginRight: "50px" }}>
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

function Alpha() {
  return (
    <div>
      <div>
      <Header />
      <Hero />
      </div>
      <div style={{ background: "#fafafa", margin: "100px auto", padding: "5px"}}>
        <Preview
          side="left"
          ttl="Quicker Sorting"
          tag="AC’s sort algorithm is significantly faster than the standard qsort and C++’s implementation."
          dsc="This is accomplished through improved detection for sorted, equal, and reversed records at little to no computational cost, and hand-unrolling heap sorts for small arrays."
          btn="ac_sort Docs"
          />
        <Preview
          ttl="Faster JSON Parsing"
          tag="Build lightning-fast services with AC’s JSON parser that can parse over 1GB of json per second per CPU."
          dsc=""
          btn="ac_json Docs"
          />
      </div>
      <Social />
      <About />
      <Footer />
    </div>
  );
}

export default Alpha;

/*
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
        </div>
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
      </div>

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
}*/
