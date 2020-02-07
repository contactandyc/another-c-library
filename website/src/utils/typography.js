import Typography from "typography";
import githubTheme from "typography-theme-github";

githubTheme.overrideThemeStyles = ({ rhythm }, options) => ({
  'ol, ul': { marginLeft: "16px" }
});

export default new Typography(githubTheme);
