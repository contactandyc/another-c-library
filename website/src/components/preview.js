import { Link } from "gatsby";
import React from "react";

import MyChart from "./charts";
import "../pages/index.css";

function Preview(props) {
  return (
    <div className="Preview Flex">
      <div className="Preview-txt">
        <div>{props.ttl}</div>
        <div>{props.sub}</div>
        <div>{props.dsc}</div>
        <Link to={props.pth}>{props.btn}</Link>
      </div>
      <div className="Preview-txt">
        <MyChart />
      </div>
    </div>
  );
}

export default Preview;

/*
  // Codebox content requires individual components
  return (
    <>
      <section className="Preview Flex">
        {txt[0]}
        <div className="Preview-cd">
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
      </section>

      <section className="Preview Flex Flex-rvrs">
        <div className="Preview-cd">
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
        {txt[1]}
      </section>

      <section className="Preview Flex">
        {txt[2]}
        <div className="Preview-cd">
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
      </section>

      <section className="Preview Flex Flex-rvrs">
        <div className="Preview-cd">
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
        {txt[3]}
      </section>
    </>
  );
}
*/
