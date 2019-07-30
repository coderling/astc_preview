Shader "ASTC/ColorMask"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
		_Color ("Color mask", color) = (1, 1, 1, 1)
		_AlphaToogle("Alpha Toggle", int) = 1

		[Toggle(UNITY_UI_ALPHACLIP)] _UseUIAlphaClip ("Use Alpha Clip", Float) = 0
	}
    SubShader
    {
		Tags
        {
            "Queue"="Transparent"
            "RenderType"="Transparent"
        }

        Pass
        {
			Cull Off
            Lighting Off
            ZWrite Off
            AlphaTest Off
            Fog { Mode Off }

			Blend SrcAlpha OneMinusSrcAlpha
            
			CGPROGRAM
			#pragma multi_compile _ UNITY_UI_CLIP_RECT
            #pragma multi_compile _ UNITY_UI_ALPHACLIP		   

            #pragma vertex vert
            #pragma fragment frag
            
			#include "UnityCG.cginc"
			#include "UnityUI.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
				float4 worldPosition : TEXCOORD1;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
			float4 _Color;
			int _AlphaToggle;
			float4 _ClipRect;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
				o.worldPosition = v.vertex;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // sample the texture
                fixed4 col = tex2D(_MainTex, i.uv);
				if (_AlphaToggle == 0)
				{
					col.a = 1;
				}

				#ifdef UNITY_UI_CLIP_RECT
				col.a *= UnityGet2DClipping(i.worldPosition.xy, _ClipRect);
				#endif

				#ifdef UNITY_UI_ALPHACLIP
				clip (col.a - 0.001);
				#endif
                
				return col *_Color;
            }
            ENDCG
        }
    }
}
