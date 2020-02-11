import React from "react";
import "../pages/index.css";

function Social() {
  return (
    <div className="Index-social Flex">
      <div className="Index-social-signup Flex">
        <form name="subscribe" method="post">
          <input type="hidden" name="subscribe" value="contact" />
          <label>Subscribe for release updates: <input className="Index-social-signup-input" type="email" name="email"/>
          </label>
          <button className="Index-social-signup-btn" type="submit">
            SIGNUP
          </button>
        </form>
      </div>
      <div className="Index-github-btns">
        <iframe title="github button" src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=fork&count=false&size=large" frameBorder="0" scrolling="0" width="158px" height="30px"></iframe>
        <iframe title="github button 2" src="https://ghbtns.com/github-btn.html?user=twbs&repo=bootstrap&type=star&count=false&size=large" frameBorder="0" scrolling="0" width="160px" height="30px"></iframe>
      </div>
    </div>
  );
}

export default Social;
