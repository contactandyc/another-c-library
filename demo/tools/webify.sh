#!/bin/bash

sed 's/  /\&nbsp; /g' | sed 's/  /\&nbsp; /g' | sed 's/"/\&quot;/g' | sed 's/{/\&#123;/g' | sed 's/{/\&#125;/g' | sed 's/\</\&lt;/g' | sed 's/\>/\&gt;/g' | sed 's#$#<br\/>#g'
