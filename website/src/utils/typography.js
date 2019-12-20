import Typography from 'typography'
import githubTheme from 'typography-theme-github'

githubTheme.overrideThemeStyles = ({ rhythm }, options) => ({
  'ol, ul': {
    marginLeft: "16px",
  }
})
const typography = new Typography(githubTheme)

export default typography
