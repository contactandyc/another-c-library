#!/bin/bash

sed 's#$#<br\/>#g' | sed 's/  /\&nbsp; /g' | sed 's/  /\&nbsp; /g' | sed 's/"/\&quot;/g'
