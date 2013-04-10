//
//  Shader.fsh
//  SSA-sample1
//
//  Created by IKEDA KOHEI on 10/05/11.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
