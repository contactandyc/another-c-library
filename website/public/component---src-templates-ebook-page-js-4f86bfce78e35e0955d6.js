(window.webpackJsonp=window.webpackJsonp||[]).push([[11],{Z9P9:function(e,t,a){"use strict";a.r(t);var n=a("q1tI"),r=a.n(n),l=a("TJpk"),i=a("Bl7J"),o=a("vrFN");var c=function(e){return r.a.createElement("div",{dangerouslySetInnerHTML:{__html:e.content}})},s=a("pMgH");a.d(t,"pageQuery",(function(){return d}));t.default=function(e){var t=e.data,a=t.markdownRemark,n=t.allMarkdownRemark,d={main:{margin:"0 auto",maxWidth:"1200px",display:"flex"},content:{color:"black",margin:"0",padding:"20px"},sidebar:{background:"#F7FAFC",height:"100vh",padding:"15px",color:"#63B3ED",position:"sticky",top:"62px"},activeLink:{color:"#38A169",fontWeight:"700"}};return r.a.createElement(i.a,null,r.a.createElement(o.a,{title:a.frontmatter.title}),r.a.createElement(l.Helmet,{title:""+a.frontmatter.title}),r.a.createElement("div",{style:d.main},r.a.createElement("div",{style:d.content,className:"Content"},r.a.createElement("h1",null,a.frontmatter.title),r.a.createElement(c,{content:a.html})),r.a.createElement(s.a,{allPages:n,type:"ebook"})))};var d="82858208"},pMgH:function(e,t,a){"use strict";a("zGcK");var n=a("q1tI"),r=a.n(n),l=a("Wbzz");t.a=function(e){for(var t={sidebar:{fontFamily:"-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen','Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif",width:"400px",height:"100vh",padding:"15px",color:"#1E4E8C",position:"sticky",top:"62px",borderLeft:"solid 1px #EDF2F7"},activeLink:{color:"#38A169",fontWeight:"700"},ul:{paddingLeft:"0",listStyle:"none"},li:{marginBottom:"0"}},a=new Intl.Collator(void 0,{numeric:!0,sensitivity:"base"}),n=[],i=[],o=0;o<e.allPages.edges.length;o++)""!==e.allPages.edges[o].node.frontmatter.title&&(n.push(e.allPages.edges[o].node.frontmatter.title),i.push(e.allPages.edges[o].node.frontmatter.path));return n.sort(a.compare),i.sort(a.compare),r.a.createElement("div",{style:t.sidebar,className:"Sidebar"},r.a.createElement("div",null,n.map((function(a,n){return r.a.createElement("div",{key:n},r.a.createElement(l.Link,{to:"/"+e.type+i[n],activeStyle:t.activeLink},a))}))))}}}]);
//# sourceMappingURL=component---src-templates-ebook-page-js-4f86bfce78e35e0955d6.js.map