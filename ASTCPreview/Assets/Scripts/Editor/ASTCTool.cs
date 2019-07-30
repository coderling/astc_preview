using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using UnityEditor;
using System.Runtime.InteropServices;

namespace ASTC
{
    /// <summary>
    /// to do
    /// 1. encode 支持
    /// </summary>
    public class ASTCTool : EditorWindow
    {
        public enum EChanelMask : int
        {
            SO = 0,
            RGB,
            R,
            G,
            B,
            Max,
        }

        public enum EASTCENCODE : int
        {
            None = 0,
            E4x4,
            E5x5,
            E6x6,
            E8x8,
            E10x10,
            E12x12,
            E4x4_A = 7,
            E5x5_A,
            E6x6_A,
            E8x8_A,
            E10x10_A,
            E12x12_A,
        }

        public enum ECompressQuality
        {
            QUALITY_VERYFAST = 0,
            QUALITY_FAST,
            QUALITY_MEDIUM,
            QUALITY_THOROUGH,
            QUALITY_EXHAUSTIVE,
        }

        [MenuItem("ASTC/OpenPreview")]
        static void OpenAstcPreView()
        {
            ASTCWrap.SetUnityError(LogError);
            EditorWindow.GetWindow<ASTCTool>("ASTC Preview");
        }

        private static void LogError(string msg)
        {
            Debug.LogError(msg);
        }

        private readonly string[] channel_button_texts = new string[(int)EChanelMask.Max];
        private const int m_initTextureMaxHeight = 256;

        Rect m_textureRect;
        Texture m_sourceTexture;
        Texture m_previewTexture;

        Material m_mat;
        EChanelMask m_mask = EChanelMask.SO;
        RenderTexture m_bgTexture;
        Vector2 m_scrollVec;
        float m_scale = 1;
        EASTCENCODE m_willEncodeType = EASTCENCODE.None;
        EASTCENCODE m_encodeType = EASTCENCODE.None;
        ECompressQuality m_quality = ECompressQuality.QUALITY_MEDIUM;
        ECompressQuality m_willQuality = ECompressQuality.QUALITY_MEDIUM;

        private void OnEnable()
        {
            Reset();
            if(Selection.activeObject != null && (Selection.activeObject is Texture || Selection.activeObject is Sprite))
            {
                Sprite sp = Selection.activeObject as Sprite;
                if(sp != null)
                {
                    m_sourceTexture = sp.texture;
                }
                else
                {
                    m_sourceTexture = Selection.activeObject as Texture;
                }
            }
            m_bgTexture = RenderTexture.GetTemporary(256, 256, 0, RenderTextureFormat.ARGB32); 
            m_mat = new Material(Shader.Find("ASTC/ColorMask"));
            for(EChanelMask i = 0; i < EChanelMask.Max; ++i)
            {
                channel_button_texts[(int)i] = i.ToString();
            }

            if (m_sourceTexture == null)
            {
                return;
            }

            m_encodeType = m_willEncodeType = EASTCENCODE.None;
            m_quality = m_willQuality = ECompressQuality.QUALITY_MEDIUM;
            InitCaculateTextureShow();
        }

        private void OnDestroy()
        {
            if(m_bgTexture != null)
            {
                RenderTexture.ReleaseTemporary(m_bgTexture);
                m_bgTexture = null;
            }
        }

        private void InitCaculateTextureShow()
        {
            if(m_sourceTexture == null)
            {
                return;
            }

            m_previewTexture = m_sourceTexture;

            if(m_sourceTexture.width > m_initTextureMaxHeight)
            {
                m_scale = m_initTextureMaxHeight * 1.0f / m_sourceTexture.width;
            }
        }

        private void OnGUI()
        {
            bool need_repaint = false;
            if(m_sourceTexture == null)
            {
                m_sourceTexture = EditorGUILayout.ObjectField("放置预览texture", m_sourceTexture, typeof(Texture), false) as Texture;
                if(m_sourceTexture != null)
                {
                    InitCaculateTextureShow();
                }
            }  
            else
            {
                if(Event.current.type == EventType.Repaint)
                {
                    EditorGUI.DrawTextureTransparent(new Rect(0, 0, this.position.width, this.position.height), m_bgTexture);
                }

                GUILayout.BeginHorizontal(EditorStyles.toolbar, GUILayout.Width(this.position.width));
                {
                    if(GUILayout.Button("Reset", EditorStyles.toolbarButton, GUILayout.Width(50)))
                    {
                        Reset();
                        Repaint();
                        return;
                    }

                    m_mask = (EChanelMask)GUILayout.SelectionGrid((int)m_mask, channel_button_texts, channel_button_texts.Length, EditorStyles.toolbarButton, GUILayout.Width(180));
                    m_willEncodeType = (EASTCENCODE)EditorGUILayout.EnumPopup(m_willEncodeType, EditorStyles.toolbarPopup, GUILayout.Width(80));


                    if(m_willEncodeType != EASTCENCODE.None)
                    {
                        m_willQuality = (ECompressQuality)EditorGUILayout.EnumPopup(m_willQuality, EditorStyles.toolbarPopup, GUILayout.Width(80));
                    }

                    if (GUILayout.Button("PreView", EditorStyles.toolbarButton, GUILayout.Width(100)))
                    {
                        Texture image;
                        if (TryRefreshEncode(m_willEncodeType, out image))
                        {
                            m_encodeType = m_willEncodeType;
                            m_quality = m_willQuality;
                            //show the astc
                            m_previewTexture = image;
                        }
                    }
                }GUILayout.EndHorizontal();

                if (Event.current.type == EventType.MouseDrag)
                {
                    m_scrollVec.x = m_scrollVec.x - Event.current.delta.x;
                    m_scrollVec.y = m_scrollVec.y - Event.current.delta.y;
                    need_repaint = true;
                }
                else if(Event.current.type == EventType.ScrollWheel)
                {
                    m_scale += Event.current.delta.y * 0.01f;
                    need_repaint = true;
                }

                Color col = new Color();
                m_mat.SetInt("_AlphaToggle", 1);
                switch (m_mask)
                {
                    case EChanelMask.R:
                        col.r = 1;
                        col.g = 0;
                        col.b = 0;
                        col.a = 1;
                        break;
                    case EChanelMask.G:
                        col.r = 0;
                        col.g = 1;
                        col.b = 0;
                        col.a = 1;
                        break;
                    case EChanelMask.B:
                        col.r = 0;
                        col.g = 0;
                        col.b = 1;
                        col.a = 1;
                        break;
                    case EChanelMask.RGB:
                        col.r = 1;
                        col.g = 1;
                        col.b = 1;
                        col.a = 1;
                        m_mat.SetInt("_AlphaToggle", 0);
                        break;
                    case EChanelMask.SO:
                        col.r = 1;
                        col.g = 1;
                        col.b = 1;
                        col.a = 1;
                        break;
                    default:
                        col.r = col.r = col.b = col.a = 1;
                        break;
                }

                m_mat.SetColor("_Color", col);
                //自定义材质，需手动开启裁剪
                m_mat.EnableKeyword("UNITY_UI_CLIP_RECT");
                m_mat.SetVector("_ClipRect", new Vector4(0, 0, this.position.width, this.position.height));
                
                float content_width = m_previewTexture.width * m_scale;
                float content_height = m_previewTexture.height * m_scale;
                float x = -m_scrollVec.x;
                float y = 25 - m_scrollVec.y;
                m_textureRect = new Rect(x, y,  content_width, content_height);
                GUI.BeginClip(new Rect(0, 16, this.position.width, this.position.height));
                EditorGUI.DrawPreviewTexture(m_textureRect, m_previewTexture, m_mat, ScaleMode.StretchToFill, 0, 0);
                EditorGUI.DrawTextureAlpha(new Rect(m_textureRect.x, m_textureRect.y + m_textureRect.height + 10, m_textureRect.width, m_textureRect.height), m_previewTexture, ScaleMode.StretchToFill, 0, 0);
                GUI.EndClip();

                

                float right = Math.Max(content_width + 30, this.position.width);
                float left = -30;
                m_scrollVec.x = GUI.HorizontalScrollbar(new Rect(0, this.position.height - 16, this.position.width - 16, 16), m_scrollVec.x, this.position.width, left, right);
                right = Math.Max(content_height * 2 + 70, this.position.height);
                left = -30;
                m_scrollVec.y = GUI.VerticalScrollbar(new Rect(this.position.width - 16, 0, 16, this.position.height), m_scrollVec.y, this.position.height, left, right);
            }

            if(need_repaint)
            {
                Repaint();
            }
        }

        private void Reset()
        {
            m_scale = 1;
            m_sourceTexture = null;
            m_previewTexture = null;
            m_willEncodeType = EASTCENCODE.None;
        }

        private string GetSourceTextureFullPath()
        {
            if(m_sourceTexture == null)
            {
                return "";
            }

            string assetPath = AssetDatabase.GetAssetPath(m_sourceTexture);
            return System.IO.Path.GetFullPath(assetPath);
        }

        private bool m_previewDirty
        {
            get
            {
                return m_willEncodeType != m_encodeType || m_quality != m_willQuality && m_encodeType != EASTCENCODE.None;
            }
        }
        private bool TryRefreshEncode(EASTCENCODE t, out Texture image)
        {
            image = null;
            if(m_sourceTexture == null || !m_previewDirty)
            {
                return false;
            }

            int sw_alpha = 4;
            bool need_pack = false;
            switch(t)
            {
                case EASTCENCODE.None:
                    //show the source
                    need_pack = false;
                    break;
                case EASTCENCODE.E4x4:
                case EASTCENCODE.E5x5:
                case EASTCENCODE.E6x6:
                case EASTCENCODE.E8x8:
                case EASTCENCODE.E10x10:
                case EASTCENCODE.E12x12:
                    need_pack = true;
                    break;
                default:
                    EditorUtility.DisplayDialog("提示", "暂不支持该格式", "确定");
                    break;
            }

            if (need_pack)
            {
                string file_path = GetSourceTextureFullPath();
                var ptr = ASTCWrap.pack_and_unpack_image(file_path, (int)t, 0, sw_alpha, 0);
                image = DecodeImageData(ptr);
            }
            return need_pack;
        }

        public unsafe static Texture2D DecodeImageData(IntPtr ptr)
        {
            ASTCWrap.astc_codec_image img = Marshal.PtrToStructure<ASTCWrap.astc_codec_image>(ptr);
            Texture2D tex = new Texture2D(img.xsize, img.ysize, TextureFormat.RGBA32, false);
            Color32[] colors = new Color32[img.xsize * img.ysize];
            for (int i = 0; i < img.ysize; i++)
            {
                for (int j = 0; j < img.xsize; j++)
                {
                    colors[i * img.xsize + j].r = img.imagedata8[0][i][4 * j];
                    colors[i * img.xsize + j].g = img.imagedata8[0][i][4 * j + 1];
                    colors[i * img.xsize + j].b = img.imagedata8[0][i][4 * j + 2];
                    colors[i * img.xsize + j].a = img.imagedata8[0][i][4 * j + 3];
                }
            }

            tex.SetPixels32(colors);
            tex.Apply(false, false);
            return tex;
        }
    }
}

