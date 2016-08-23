<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="/UnitTest">
    <html>
      <style>
      </style>
      <body>
        <h2>Predict Unit Test Summary</h2>
        <h3>Assert 'absolute' % error Legend [(actual - expected) / expected] </h3>
        <table>
          <tr>
            <td>Less than 0.1%</td>
            <td width="100" bgcolor="#33cc33"/>
          </tr>
          <tr>
            <td>Less than 0.5%</td>
            <td width="100" bgcolor="#66ff33"/>
          </tr>
          <tr>
            <td>Less than 1%</td>
            <td width="100" bgcolor="#ccff33"/>
          </tr>
          <tr>
            <td>Less than 2%</td>
            <td width="100" bgcolor="#ffff66"/>
          </tr>
          <tr>
            <td>Less than 5%</td>
            <td width="100" bgcolor="#ffff99"/>
          </tr>
          <tr>
            <td>Less than 10%</td>
            <td width="100" bgcolor="#ff9933"/>
          </tr>
          <tr>
            <td>Less than 20%</td>
            <td width="100" bgcolor="#ff3300"/>
          </tr>
          <tr>
            <td>Greater than 20%</td>
            <td width="100" bgcolor="#8D1919"/>
          </tr>
        </table>
        <h2>Test Details</h2>
        <table border="1">
          <tr bgcolor="#9acd32">
            <th width="200">Number of Tests</th>
            <th width="200">Number of Successes</th>
            <th width="200">Number of Failures</th>
            <th width="200">Time of Execution</th>
          </tr>
          <xsl:for-each select="Summary">
            <tr>
              <td>
                <xsl:value-of select="Methods"/>
              </td>
              <td>
                <xsl:value-of select="Success"/>
              </td>
              <td>
                <xsl:value-of select="Failure"/>
              </td>
              <td>
                <xsl:value-of select="Time"/>
              </td>
            </tr>
          </xsl:for-each>
        </table>
        <table border="1">
          <tr bgcolor="#9acd32">
            <th width="200">Class</th>
            <th width="200">Test</th>
            <th width="100">Result</th>
            <th width="400">Time/Reason</th>
          </tr>
          <xsl:for-each select="Tests/Class">
            <tr>
              <td bgcolor="#e8e8e8">
                <xsl:value-of select="Name"/>
              </td>
              <td bgcolor="#e8e8e8"> </td>
              <td bgcolor="#e8e8e8"> </td>
              <td bgcolor="#e8e8e8"> </td>
              <xsl:for-each select="Method">
                <tr>
                  <td>
                  </td>
                  <td>
                    <xsl:value-of select="Name"/>
                  </td>
                  <xsl:if  test="Result='Failed'">
                    <td bgcolor="#B80000">
                      <xsl:value-of select="Result" />
                    </td>
                    <td>
                      <table>
                        <xsl:for-each select="Asserts/Assert">
                          <tr width="475">
                            <td width="375">
                              <xsl:value-of select="Message"/>
                            </td>
                            <xsl:choose>
                              <xsl:when test="Diff &lt; 0.1" >
                                <td width="100" bgcolor="#33cc33">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 0.5" >
                                <td width="100" bgcolor="#66ff33">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 1" >
                                <td width="100" bgcolor="#ccff33">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 2" >
                                <td width="100" bgcolor="#ffff66">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 5" >
                                <td width="100" bgcolor="#ffff99">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 10" >
                                <td width="100" bgcolor="#ff9933">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &lt; 20" >
                                <td width="100" bgcolor="#ff3300">
                                  <xsl:value-of select="Diff"/>%
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff = 2415919104" >
                                <td width="100" bgcolor="#8D1919">
                                  <font color="white">
                                  Infinity
                                  </font>
                                </td>
                              </xsl:when>
                              <xsl:when test="Diff &gt; 20" >
                                <td width="100" bgcolor="#8D1919">
                                  <font color="white">
                                  <xsl:value-of select="Diff"/>%
                                  </font>
                                </td>
                              </xsl:when>
                            </xsl:choose>
                          </tr>
                        </xsl:for-each>
                      </table>
                    </td>
                  </xsl:if>
                  <xsl:if  test="Result='Success'">
                    <td>
                      <xsl:value-of select="Result" />
                    </td>
                    <td>
                      <xsl:value-of select="Time"/>
                    </td>
                  </xsl:if>
                </tr>
              </xsl:for-each>
            </tr>
          </xsl:for-each>
        </table>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>