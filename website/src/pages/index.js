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
      <h2 className="Index-h2-alt">A Map/Reduce Framework in C</h2>
      <h3 className="Index-h3-alt">The speed of C mixed with the very useful map/reduce pattern</h3>
      <p>The AC map/reduce framework is a library and as such you maintain control of every layer of complexity.</p>
      <Link to="/docs/ac-mapreduce">
        <button className="Index-btn" style={{ fontSize: "16px", margin: "10px 10px 10px 0" }}>
          Read More
        </button>
      </Link>
      </div>
      <div className="Flex Index-mn">
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Quicker Sorting</h2>
          <h3 className="Index-h3-alt">AC’s sort algorithm is significantly faster than the standard qsort and C++’s implementation.</h3>
          <p>This is accomplished through improved detection for sorted, equal, and reversed records at little to no computational cost, and hand-unrolling heap sorts for small arrays.</p>
          <div className="Flex">
            <Link to="/docs/ac-sort">
              <button className="Index-btn" style={{ fontSize: "16px", margin: "10px 10px 10px 0" }}>
                Documentation
              </button>
            </Link>
            <Link to="/docs/improving-quicksort">
              <button className="Index-btn" style={{ fontSize: "16px" }}>
                Read Article
              </button>
            </Link>
          </div>
        </div>
        <div className="Index-cd-alt">
demo/sort $ make<br/>
ac_sort vs std::sort<br/>
Ordered test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; std::sort&nbsp; &nbsp; &nbsp;&nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.043&nbsp; &nbsp; &nbsp;&nbsp; 0.341&nbsp; &nbsp; &nbsp; 693.023%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 3.228&nbsp; &nbsp; &nbsp; 29.921&nbsp; &nbsp; &nbsp; 826.921%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 393.470&nbsp;&nbsp; 3,315.230&nbsp; &nbsp; &nbsp; 742.562%<br/>
<br/>
Equal test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; std::sort&nbsp; &nbsp; &nbsp;&nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.045&nbsp; &nbsp; &nbsp;&nbsp; 0.304&nbsp; &nbsp; &nbsp; 575.556%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 3.154&nbsp; &nbsp; &nbsp; 25.942&nbsp; &nbsp; &nbsp; 722.511%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 384.580&nbsp;&nbsp; 3,202.080&nbsp; &nbsp; &nbsp; 732.617%<br/>
<br/>
Reverse test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; std::sort&nbsp; &nbsp; &nbsp;&nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.084&nbsp; &nbsp; &nbsp;&nbsp; 0.536&nbsp; &nbsp; &nbsp; 538.095%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 6.197&nbsp; &nbsp; &nbsp; 56.242&nbsp; &nbsp; &nbsp; 807.568%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 963.120&nbsp;&nbsp; 5,984.830&nbsp; &nbsp; &nbsp; 521.400%<br/>
<br/>
Random test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; std::sort&nbsp; &nbsp; &nbsp;&nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.388&nbsp; &nbsp; &nbsp;&nbsp; 1.576&nbsp; &nbsp; &nbsp; 306.186%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; 411.915&nbsp; &nbsp;&nbsp; 506.590&nbsp; &nbsp; &nbsp;&nbsp; 22.984%<br/>
1,000,000&nbsp;&nbsp; 60,816.670&nbsp; 77,113.870&nbsp; &nbsp; &nbsp;&nbsp; 26.797%<br/>
<br/>
ac_sort vs qsort<br/>
Ordered test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; qsort&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.043&nbsp; &nbsp; &nbsp; &nbsp; 0.486&nbsp; &nbsp;&nbsp; 1030.233%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 3.228&nbsp; &nbsp; &nbsp;&nbsp; 40.842&nbsp; &nbsp;&nbsp; 1165.242%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 393.470&nbsp; &nbsp; 4,379.690&nbsp; &nbsp;&nbsp; 1013.094%<br/>
<br/>
Equal test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; qsort&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.045&nbsp; &nbsp; &nbsp; &nbsp; 0.294&nbsp; &nbsp; &nbsp; 553.333%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 3.154&nbsp; &nbsp; &nbsp;&nbsp; 28.190&nbsp; &nbsp; &nbsp; 793.786%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 384.580&nbsp; &nbsp; 2,784.150&nbsp; &nbsp; &nbsp; 623.946%<br/>
<br/>
Reverse test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; qsort&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.084&nbsp; &nbsp; &nbsp; &nbsp; 2.212&nbsp; &nbsp;&nbsp; 2533.333%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; &nbsp; 6.197&nbsp; &nbsp; &nbsp; 216.290&nbsp; &nbsp;&nbsp; 3390.237%<br/>
1,000,000&nbsp; &nbsp; &nbsp; 963.120&nbsp;&nbsp; 22,208.090&nbsp; &nbsp;&nbsp; 2205.849%<br/>
<br/>
Random test (time in microseconds)<br/>
items&nbsp; &nbsp; &nbsp;&nbsp; ac_sort&nbsp; &nbsp;&nbsp; qsort&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; % gain&nbsp; <br/>
&nbsp; &nbsp; &nbsp; 100&nbsp; &nbsp; &nbsp; &nbsp; 0.388&nbsp; &nbsp; &nbsp; &nbsp; 1.952&nbsp; &nbsp; &nbsp; 403.093%<br/>
&nbsp;&nbsp; 10,000&nbsp; &nbsp; &nbsp; 411.915&nbsp; &nbsp; &nbsp; 777.368&nbsp; &nbsp; &nbsp;&nbsp; 88.720%<br/>
1,000,000&nbsp;&nbsp; 60,816.670&nbsp; 111,768.980&nbsp; &nbsp; &nbsp;&nbsp; 83.780%<br/>
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
          <Link to="/docs/ac-json">
            <button className="Index-btn" style={{ fontSize: "16px", margin: 0 }}>
              Documentation
            </button>
          </Link>
        </div>
      </div>

      <div className="Flex Index-mn">
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Reduce Memory Errors</h2>
          <h3 className="Index-h3-alt">AC’s allocator aids in development as memory leaks, double frees, and incorrect frees are reported.  The allocation aids can be compiled out for production.  Often programs will run at 90-99% of the optimized speed with the allocation aids enabled.</h3>
          <div className="Flex">
            <Link to="/docs/ac-allocator">
              <button className="Index-btn" style={{ fontSize: "16px", margin: "10px 10px 10px 0" }}>
                Documentation
              </button>
            </Link>
          </div>
        </div>
        <div className="Index-cd-alt">
#include &quot;ac_allocator.h&quot;<br/>
<br/>
#include &lt;stdio.h&gt;<br/>
<br/>
int main(int argc, char *argv[]) &#123;<br/>
&nbsp; printf(&quot;Demo to show how allocations are tracked\n&quot;);<br/>
&nbsp; char *s = ac_strdup(argv[0]);<br/>
&nbsp; // ac_free(s);<br/>
&nbsp; return 0;<br/>
&#125;<br/>
<br/>
$ ./detecting_memory_loss<br/>
Demo to show how allocations are tracked<br/>
24 byte(s) allocated in 1 allocations (40 byte(s) overhead)<br/>
detecting_memory_loss.c:7: 24 <br/>
        </div>
      </div>

      <div className="Flex Index-mn">
        <div className="Index-cd-alt">
#include &quot;ac_buffer.h&quot;<br/>
#include &quot;ac_pool.h&quot;<br/>
<br/>
#include &lt;stdio.h&gt;<br/>
<br/>
int main(int argc, char *argv[]) &#123;<br/>
&nbsp; ac_pool_t *pool = ac_pool_init(65536);<br/>
&nbsp; char *cmd = ac_pool_strdupf(pool, &quot;CMD: %s&quot;, argv[0]);<br/>
&nbsp; printf(&quot;%s\n&quot;, cmd);<br/>
&nbsp; ac_buffer_t *bh = ac_buffer_pool_init(pool, 256);<br/>
&nbsp; ac_buffer_setf(bh, &quot;The buffer will be destroyed when pool is destroyed!&quot;);<br/>
&nbsp; printf(&quot;%s\n&quot;, ac_buffer_data(bh));<br/>
&nbsp; ac_pool_destroy(pool);<br/>
&nbsp; return 0;<br/>
}<br/>
<br/>
$ ./pool_demo<br/>
CMD: ./pool_demo<br/>
The buffer will be destroyed when pool is destroyed!<br/>
                </div>
        <div className="Index-mn-cnt">
          <h2 className="Index-h2-alt">Garbage Collection on Your Terms</h2>
          <h3 className="Index-h3-alt">The JSON parser performance can partially be attributed to the ac_pool object.  The pool object removes the need to track allocations and lets you decide when memory should be collected.  The pool makes it easy to handle allocation!</h3>
          <Link to="/docs/ac-pool">
            <button className="Index-btn" style={{ fontSize: "16px", margin: 0 }}>
              Documentation
            </button>
          </Link>
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
