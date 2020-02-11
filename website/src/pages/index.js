import React from "react";

import Layout from "../components/layout";
import SEO from "../components/seo";
import Preview from "../components/preview";
import About from "../components/about";
import Social from "../components/social";
import Hero from "../components/hero";
import "./index.css";

function IndexPage() {
  const content = [
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

  return (
    <Layout>
      <SEO title="Build complex, scalable applications" />
      <Hero />
      <div className="Index-preview">
        {content.map((val, i) => {
          return (
            <Preview
              key={i}
              i={i}
              ttl={content[i].ttl}
              sub={content[i].sub}
              dsc={content[i].dsc}
              btn={content[i].btn}
              pth={content[i].pth}
            />
          );
        })}
      </div>
      <Social />
      <About />
    </Layout>
  );
}

export default IndexPage;
