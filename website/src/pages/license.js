import React from "react";
import { FaExternalLinkAlt } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import "./index.css";

function License() {
  return (
    <Layout>
      <SEO title="License" />
      <div className="Content-basic">
        <h1>License</h1>
        <p>
          Our goal is to produce a work that is unencumbered by licenses, and
          hope that others will find this code or parts of it useful. This
          software and documentation contains code and ideas derived from works
          published by Robert Sedgewick relating to the Red-Black Tree and the
          Quicksort algorithm. Many other works (both code and explanations)
          were studied. I've spent years studying many open source
          implementations including the details of the linux kernel, so I'm sure
          you will see their influence in how the code is written. I have tried
          to write the code without borrowing any code or explanations from
          other projects, but I'm sure that approaches, variable names, etc will
          look similar. I've worked with brilliant engineers over the years and
          certainly borrowed approaches from their work. I will try and call out
          where I have learned things when it is relevant (and I remember). I'm
          sure that there are better ways to do things and welcome help!
        </p>
        <p>
          Throughout the tutorial and documentation on this website, the
          following notice will appear at the top of most of the files:
        </p>
        <p><strong>Copyright 2019 Andy Curtis & Daniel Curtis</strong></p>
        <p>
          Licensed under the Apache License, Version 2.0 (the "License"); you
          may not use this file except in compliance with the License. You may
          obtain a copy of the License at:
        </p>
        <a
          href="https://www.apache.org/licenses/LICENSE-2.0"
          target="_blank"
          rel="noopener noreferrer"
          >
          https://www.apache.org/licenses/LICENSE-2.0 <FaExternalLinkAlt />
        </a>
        <br /><br />
        <p>
          Unless required by applicable law or agreed to in writing, software
          distributed under the License is distributed on an "AS IS" BASIS,
          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
          implied. See the License for the specific language governing
          permissions and limitations under the License.
        </p>
        <p>
          The documentation website was built using React, GatsbyJS, GraphQL,
          and React Icons which are all licensed under the MIT License.
        </p>
      </div>
    </Layout>
  );
}

export default License;
