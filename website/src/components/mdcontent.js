import React from "react"

function MdContent(props) {
  return <div dangerouslySetInnerHTML={{ __html: props.content }} />
}

export default MdContent
